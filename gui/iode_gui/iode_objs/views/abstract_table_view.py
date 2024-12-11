from PySide6.QtCore import Qt, QRegularExpression, QModelIndex, Signal, Slot
from PySide6.QtGui import (QTextCursor, QTextCharFormat, QFont, QKeySequence, QKeyEvent, 
                           QTextDocument, QTextFrameFormat, QShortcut, QRegularExpressionValidator)
from PySide6.QtWidgets import (QTableView, QLineEdit, QMessageBox, QDialog, QSizePolicy, 
                               QAbstractScrollArea, QAbstractItemView)
from PySide6.QtPrintSupport import QPrinter, QPrintPreviewDialog

from iode_gui.abstract_main_window import AbstractMainWindow
from iode_objs.models.abstract_table_model import IodeAbstractTableModel
from iode_objs.delegates.base_delegate import BaseDelegate
from iode_objs.edit.edit_vars_sample import EditIodeSampleDialog

from iode import IodeType, variables
# TODO : add MAX_LENGTH_NAME (= K_MAX_NAME) to the Python iode package
MAX_LENGTH_NAME = 20


class IodeAbstractTableView(QTableView):
    """
    This class represents a custom abstract table view in the Iode application.

    It inherits from QTableView and provides additional functionality
    such as filtering, printing, and handling of object names.
    """
    database_modified = Signal()
    show_objs_request = Signal(IodeType, list)

    def __init__(self, iode_type: IodeType, delegate: BaseDelegate, parent=None):
        super().__init__(parent)

        self.iode_type: IodeType = iode_type
        self.delegate: BaseDelegate = delegate
        self.edit_dialog: QDialog = None
        self.main_window: AbstractMainWindow = None
        self.printer: QPrinter = QPrinter()
        self.document: QTextDocument = QTextDocument(self)
        self.section_edited: int = None

        stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }"

        # size policy
        size_policy_table = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        size_policy_table.setHorizontalStretch(0)
        size_policy_table.setVerticalStretch(0)
        size_policy_table.setHeightForWidth(self.sizePolicy().hasHeightForWidth())
        self.setSizePolicy(size_policy_table)
        self.setSizeAdjustPolicy(QAbstractScrollArea.SizeAdjustPolicy.AdjustIgnored)

        # sorting
        self.setSortingEnabled(True)

        # stylesheet
        self.setStyleSheet(stylesheet)

        # delegate
        self.setItemDelegate(delegate)

        # ---- Selection ----
        # See: - https://doc.qt.io/qt-5/model-view-programming.html#handling-selections-in-item-views
        #      - https://doc.qt.io/qt-5/model-view-programming.html#selections

        # -- Behavior --
        # Selecting only rows.
        # See: https://doc.qt.io/qt-5/qabstractitemview.html#SelectionBehavior-enum
        self.setSelectionBehavior(QTableView.SelectionBehavior.SelectRows)
        # -- Mode --
        # When the user selects an item in the usual way, the selection is cleared and the new item selected.
        # However, if the user presses the Ctrl key when clicking on an item, the clicked item gets toggled
        # and all other items are left untouched.
        # If the user presses the Shift key while clicking on an item, all items between the current item and
        # the clicked item are selected or unselected, depending on the state of the clicked item.
        # Multiple items can be selected by dragging the mouse over them.
        # See: https://doc.qt.io/qt-5/qabstractitemview.html#SelectionMode-enum
        self.setSelectionMode(QTableView.SelectionMode.ExtendedSelection)

        # ---- Edit Trigger ----
        # the user must double click to edit a cell
        self.setEditTriggers(QAbstractItemView.EditTrigger.DoubleClicked)

        # ---- Signals & Slots ----
        self.delete_shortcut = QShortcut(QKeySequence.StandardKey.Delete, self)
        # NOTE: Required to avoid confusion when deleting a file/directory from the File Explorer
        self.delete_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.delete_shortcut.activated.connect(self.remove_objects)

        # for editing IODE object names
        self.object_name_edit: QLineEdit = QLineEdit(self)
        if self.iode_type == IodeType.COMMENTS:
            accepted_letters = "A-Za-z"  
        elif self.iode_type == IodeType.SCALARS:
            accepted_letters = "a-z"
        else:
            accepted_letters = "A-Z"
        re = QRegularExpression(f"[{accepted_letters}_][0-9{accepted_letters}_]{{0,{MAX_LENGTH_NAME - 1}}}")
        validator = QRegularExpressionValidator(re, self)
        self.object_name_edit.setValidator(validator)
        self.object_name_edit.setHidden(True)

        self.verticalHeader().sectionDoubleClicked.connect(self.open_editor_name)
        self.object_name_edit.returnPressed.connect(self.edit_name)
        self.object_name_edit.editingFinished.connect(self.close_name_editor)

        # to popup an editing dialog box
        self.shortcutEditEnter = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_Enter), self)
        self.shortcutEditEnter.activated.connect(self.edit_obj)

        self.shortcutEditReturn = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_Return), self)
        self.shortcutEditReturn.activated.connect(self.edit_obj)

        # gets objects of the same name or listed in the CLEC structure
        self.same_cmt_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.ALT | Qt.Key.Key_F1), self)
        self.same_eq_shortcut  = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.ALT | Qt.Key.Key_F2), self)
        self.same_idt_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.ALT | Qt.Key.Key_F3), self)
        self.same_lst_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.ALT | Qt.Key.Key_F4), self)
        self.same_tbl_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.ALT | Qt.Key.Key_F6), self)
        self.vars_from_CLEC_shortcut     = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.ALT | Qt.Key.Key_F7), self)
        self.scalars_from_CLEC_shortcut  = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.ALT | Qt.Key.Key_F5), self)

        self.same_cmt_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.same_eq_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.same_idt_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.same_lst_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.same_tbl_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.vars_from_CLEC_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.scalars_from_CLEC_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)

        self.same_cmt_shortcut.activated.connect(lambda: self.show_same_name_obj_or_objs_from_CLEC(IodeType.COMMENTS))
        self.same_eq_shortcut.activated.connect(lambda: self.show_same_name_obj_or_objs_from_CLEC(IodeType.EQUATIONS))
        self.same_idt_shortcut.activated.connect(lambda: self.show_same_name_obj_or_objs_from_CLEC(IodeType.IDENTITIES))
        self.same_lst_shortcut.activated.connect(lambda: self.show_same_name_obj_or_objs_from_CLEC(IodeType.LISTS))
        self.same_tbl_shortcut.activated.connect(lambda: self.show_same_name_obj_or_objs_from_CLEC(IodeType.TABLES))
        self.vars_from_CLEC_shortcut.activated.connect(lambda: self.show_same_name_obj_or_objs_from_CLEC(IodeType.VARIABLES))
        self.scalars_from_CLEC_shortcut.activated.connect(lambda: self.show_same_name_obj_or_objs_from_CLEC(IodeType.SCALARS))

        # get list of related objects
        self.related_cmt_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_F1), self)
        self.related_eq_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_F2), self)
        self.related_idt_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_F3), self)
        self.related_lst_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_F4), self)
        self.related_scl_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_F5), self)
        self.related_tbl_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_F6), self)
        self.related_var_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_F7), self)

        self.related_cmt_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.related_eq_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.related_idt_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.related_lst_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.related_scl_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.related_tbl_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.related_var_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)

        self.related_cmt_shortcut.activated.connect(lambda: self.show_related_objs(IodeType.COMMENTS))
        self.related_eq_shortcut.activated.connect(lambda: self.show_related_objs(IodeType.EQUATIONS))
        self.related_idt_shortcut.activated.connect(lambda: self.show_related_objs(IodeType.IDENTITIES))
        self.related_lst_shortcut.activated.connect(lambda: self.show_related_objs(IodeType.LISTS))
        self.related_scl_shortcut.activated.connect(lambda: self.show_related_objs(IodeType.SCALARS))
        self.related_tbl_shortcut.activated.connect(lambda: self.show_related_objs(IodeType.TABLES))
        self.related_var_shortcut.activated.connect(lambda: self.show_related_objs(IodeType.VARIABLES))

    def setup(self, main_window: AbstractMainWindow):
        """
        Setup function for the view.

        It resets the underlying model.
        """
        self.main_window = main_window

    def update(self):
        """
        Update function for the view.

        It resets the underlying model and updates the view.
        """
        table_model: IodeAbstractTableModel = self.model()
        table_model.reset()
        self.viewport().update()

    def set_filter_line_edit(self, filter_line_edit: QLineEdit):
        """
        Set the filter line edit for this view.

        Args:
            filter_line_edit (QLineEdit): The filter line edit widget.
        """
        self.filter_line_edit = filter_line_edit

    def preferred_height(self):
        """
        Calculate and return the preferred height for the view.

        Returns:
            int: The preferred height for the view.
        """
        height = self.horizontalHeader().height()
        for i in range(self.model().rowCount()):
            height += self.rowHeight(i)
        if self.iode_type == IodeType.VARIABLES:
            height += 20
        return height

    def enable_delete_shortcut(self, enable: bool):
        """
        Enable or disable the delete shortcut.

        Args:
            enable (bool): True to enable the shortcut, False to disable it.
        """
        self.delete_shortcut.setEnabled(enable)

    def check_global_sample(self):
        """
        Check the global sample and ask to set it if not already defined.

        Returns:
            bool: True if the sample is set, False otherwise.
        """
        # check global sample and ask to set it if not already defined
        if not variables.nb_periods:
            reply = QMessageBox.question(None, "Sample", "Sample undefined. Set it?",
                                         QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No)
            if reply == QMessageBox.StandardButton.Yes:
                try:
                    dialog = EditIodeSampleDialog(self)
                    if dialog.exec() == QDialog.DialogCode.Accepted:
                        from_str = dialog.from_period
                        to_str = dialog.to_period
                        variables.sample = from_str, to_str
                    return True
                except Exception as e:
                    QMessageBox.warning(None, "WARNING", str(e))
                    return False
            else:
                return False

    def filter(self, silent=False):
        """
        Filter the view based on the text in the filter line edit.

        Args:
            silent (bool): If True, the filter operation is silent.
        """
        pattern = self.filter_line_edit.text().strip()
        table_model: IodeAbstractTableModel = self.model()
        table_model.filter(pattern, silent)
        self.update()

    def get_selected_objects_names(self) -> list:
        """
        Get the names of the selected objects in the view.

        Returns:
            list: A list of names of the selected objects.
        """
        names = []
        selection = self.selectionModel().selectedRows()
        if not selection:
            return names

        model_obj: IodeAbstractTableModel = self.model()
        names = [model_obj.headerData(index.row(), Qt.Orientation.Vertical, 
                                      Qt.ItemDataRole.DisplayRole) for index in selection]
        return names

    # override QTableView method
    @Slot(QKeyEvent)
    def keyPressEvent(self, event: QKeyEvent):
        """
        Handle key press events.

        Args:
            event (QKeyEvent): The key press event.
        """
        # hides the QLineEdit to edit the name of an IODE object if the user pressed ESCAPE
        if self.object_name_edit.isVisible() and event.key() == Qt.Key.Key_Escape:
            self.object_name_edit.setHidden(True)
            self.object_name_edit.setText("")

        # scrolls to the first row in which the IODE object name starts with the pressed key
        if not self.object_name_edit.isVisible() and len(event.text()):
            c = event.text()[0]
            if (c.isalpha() and event.count() == 1) or c == '_':
                row = 0
                while row < self.verticalHeader().count():
                    name:str = self.model().headerData(row, Qt.Orientation.Vertical)
                    if name.startswith((c, c.swapcase())):
                        self.scrollTo(self.model().index(row, 0))
                        self.selectRow(row)
                        break
                    row += 1
                return

        super().keyPressEvent(event)

    def dump_table_in_document(self):
        """
        Dump the displayed table into the document.

        Note:
            See https://forum.qt.io/post/460045
            and https://gis.stackexchange.com/q/385616
        """
        self.document.clear()
        max_nb_columns = 10

        # Get the table model
        table_model: IodeAbstractTableModel = self.model()
        nb_rows = table_model.rowCount()
        nb_cols_total = table_model.columnCount()

        # Calculate the number of tables
        nb_tables =  nb_cols_total // max_nb_columns
        if nb_cols_total % max_nb_columns > 0:
            nb_tables += 1

        cursor = QTextCursor(self.document)

        for i_table in range(nb_tables):
            nb_cols = max_nb_columns if i_table < nb_tables - 1 else nb_cols_total % max_nb_columns
            table = cursor.insertTable(nb_rows + 1, nb_cols + 1)
            table_format = table.format()
            table_format.setHeaderRowCount(1)
            table_format.setCellPadding(4)
            table_format.setCellSpacing(0)
            table_format.setBorder(0)
            table.setFormat(table_format)

            bold_format = QTextCharFormat()
            bold_format.setFontWeight(QFont.Weight.Bold)

            # Set the top left cell
            header_cell = table.cellAt(0, 0)
            cursor = header_cell.firstCursorPosition()
            cursor.insertText("Name", bold_format)
            cell_format = header_cell.format().toTableCellFormat()
            cell_format.setBottomBorderStyle(QTextFrameFormat.BorderStyle.BorderStyle_Solid)
            header_cell.setFormat(cell_format)

            # Set columns header
            for col in range(nb_cols):
                data_col = (i_table * max_nb_columns) + col
                text = table_model.headerData(data_col, Qt.Orientation.Horizontal, Qt.ItemDataRole.DisplayRole)
                header_cell = table.cellAt(0, col + 1)
                cursor = header_cell.firstCursorPosition()
                cursor.insertText(text, bold_format)
                cell_format = header_cell.format().toTableCellFormat()
                cell_format.setBottomBorderStyle(QTextFrameFormat.BorderStyle.BorderStyle_Solid)
                header_cell.setFormat(cell_format)

            # Set table rows
            for row in range(nb_rows):
                # Row header
                text = table_model.headerData(row, Qt.Orientation.Vertical, Qt.ItemDataRole.DisplayRole)
                header_cell = table.cellAt(row + 1, 0)
                cursor = header_cell.firstCursorPosition()
                cursor.insertText(text)

                # Row cells content
                for col in range(nb_cols):
                    data_col = (i_table * max_nb_columns) + col
                    text = table_model.data(table_model.index(row, data_col), Qt.ItemDataRole.DisplayRole)
                    cell = table.cellAt(row + 1, col + 1)
                    cursor = cell.firstCursorPosition()
                    cursor.insertText(text)

            cursor.movePosition(QTextCursor.MoveOperation.End)
            if i_table < nb_tables - 1:
                cursor.insertText("\n\n")

    @Slot()
    def render_for_printing(self):
        """Renders the document for printing."""
        self.document.print(self.printer)

    @Slot()
    def remove_objects(self):
        """Removes the selected objects."""
        selection = self.selectionModel().selectedRows()
        # Note: goes in reverse order because each deleted row shifts all the next rows
        for index in reversed(selection):
            self.model().removeRow(index.row(), QModelIndex())
        self.update()

    @Slot(int)
    def open_editor_name(self, section: int):
        """Opens the editor for the name of the object."""
        # This block sets up the geometry for the line edit
        v_header = self.verticalHeader()
        y = self.horizontalHeader().height() + v_header.sectionViewportPosition(section)
        geometry = (0, y, v_header.width(), v_header.sectionSize(section))
        self.object_name_edit.setGeometry(geometry)

        name = self.model().headerData(section, Qt.Orientation.Vertical)
        self.object_name_edit.setText(name)

        self.object_name_edit.setHidden(False)
        self.object_name_edit.setFocus()
        self.object_name_edit.selectAll()
        self.section_edited = section

    @Slot()
    def edit_name(self):
        """Edits the name of the object."""
        old_name = self.model().headerData(self.section_edited, Qt.Orientation.Vertical)
        new_name = self.object_name_edit.text()
        if new_name != old_name:
            self.model().setHeaderData(self.section_edited, Qt.Orientation.Vertical, new_name)

        self.object_name_edit.setHidden(True)
        self.object_name_edit.setText("")
        self.setFocus()

    @Slot()
    def close_name_editor(self):
        """Closes the editor for the name of the object."""
        self.object_name_edit.setHidden(True)
        self.object_name_edit.setText("")

    @Slot()
    def new_obj(self):
        """Creates a new object."""
        self.open_add_dialog()

    @Slot()
    def edit_obj(self):
        """Edits the selected object."""
        self.open_edit_dialog()

    @Slot(str)
    def new_object_inserted(self, name: str):
        """Handles the insertion of a new object."""
        table_model: IodeAbstractTableModel = self.model()
        if table_model.filter_active:
            pattern = self.filter_line_edit.text().strip()
            pattern += ";" + name
            self.filter_line_edit.setText(pattern)

    @Slot(str)
    def object_removed(self, name: str):
        """Handles the removal of an object."""
        table_model: IodeAbstractTableModel = self.model()
        if table_model.filter_active:
            pattern = self.filter_line_edit.text().strip()
            name = ";" + name
            if name in pattern:
                pattern.replace(name, "")
            self.filter_line_edit.setText(pattern)

    @Slot()
    def filter_slot(self):
        """Filters the objects."""
        self.filter()

    @Slot()
    def print(self):
        """Prints the objects."""
        self.printer = QPrinter()
        dialog = QPrintPreviewDialog(self.printer, self)
        dialog.paintRequested.connect(self.render_for_printing)
        self.dump_table_in_document()
        dialog.exec()

    def _open_edit_dialog(self, obj_name: str):
        raise NotImplementedError()

    @Slot()
    def open_edit_dialog(self):
        selection = self.selectionModel().selectedRows()
        if len(selection) == 0:
            return

        section = selection[0].row()
        model: IodeAbstractTableModel = self.model()
        obj_name = model.headerData(section, Qt.Orientation.Vertical)

        if self.edit_dialog:
            self.edit_dialog.close()

        self._open_edit_dialog(obj_name)
        if self.edit_dialog:
            self.edit_dialog.show()

    def _open_add_dialog(self) -> QDialog:
        raise NotImplementedError()

    @Slot()
    def open_add_dialog(self):
        model: IodeAbstractTableModel = self.model()
        add_dialog = self._open_add_dialog()
        add_dialog.new_object_inserted.connect(self.new_object_inserted)
        if add_dialog.exec() == QDialog.DialogCode.Accepted:
            model.reset()

    @Slot(IodeType)
    def show_same_name_obj_or_objs_from_CLEC(self, other_type: IodeType):
        """
        Shows equations, identities, scalars or variables listed in the CLEC
        structure or the object of the same name.

        Parameters:
        other_type: The type of the object to show.
        """
        selection = self.selectionModel().selectedRows()
        if not selection:
            return
        index = selection[0]

        table_model: IodeAbstractTableModel = self.model()
        name = table_model.headerData(index.row(), Qt.Orientation.Vertical, Qt.ItemDataRole.DisplayRole)

        list_objs = table_model.get_same_name_obj_or_objs_from_CLEC(name, other_type)
        if list_objs is not None and len(list_objs):
            self.show_objs_request.emit(other_type, list_objs)

    @Slot(IodeType)
    def show_related_objs(self, other_type: IodeType):
        """
        Shows all related objects.

        Parameters:
        other_type: The type of the object to show.
        """
        selection = self.selectedIndexes()
        if not selection:
            return
        index = selection[0]

        table_model: IodeAbstractTableModel = self.model()
        name = table_model.headerData(index.row(), Qt.Orientation.Vertical, Qt.ItemDataRole.DisplayRole)

        list_objs = table_model.get_related_objs(name, other_type)
        if list_objs is not None and len(list_objs):
            self.show_objs_request.emit(other_type, list_objs)
