from PySide6.QtCore import Qt, Slot, QModelIndex, QItemSelectionRange
from PySide6.QtWidgets import QMenu, QApplication, QMessageBox, QDialog
from PySide6.QtGui import (QKeySequence, QAction, QShortcut, QContextMenuEvent, 
                           QClipboard, QGuiApplication)

from utils import NAN_REP
from iode_objs.edit.edit_vars_sample import EditIodeSampleDialog
from iode_objs.models.abstract_table_model import IodeAbstractTableModel

from typing import List, Tuple
from iode import variables, Sample


class NumericalTableView:
    def __init__(self):
        self.allow_to_paste: bool = None
        self.context_menu: QMenu = None

    # NOTE: need a setup() method since NumericalTableView.__init__() seems to be called automatically inside 
    #       super().__init__(parent) (i.e. called inside QTableView.__init__(parent)) in the beginning of 
    #       IodeAbstractTableView.__init__()
    def setup(self, allow_to_paste: bool):
        self.shortcut_copy = QShortcut(QKeySequence.StandardKey.Copy, self)
        self.shortcut_copy.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.shortcut_copy.activated.connect(self.copy)

        self.shortcut_copy_including_headers = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.SHIFT | Qt.Key.Key_C), self)
        self.shortcut_copy_including_headers.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.shortcut_copy_including_headers.activated.connect(self.copy_including_headers)

        if self.allow_to_paste:
            self.shortcut_past = QShortcut(QKeySequence.StandardKey.Paste, self)
            self.shortcut_past.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
            self.shortcut_past.activated.connect(self.paste)

            self.shortcut_past_vars = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.SHIFT | Qt.Key.Key_V), self)
            self.shortcut_past_vars.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
            self.shortcut_past_vars.activated.connect(self.paste_vars)

        self.setup_context_menu()

    def setup_context_menu(self):
        self.context_menu = QMenu(self)

        action = QAction("Copy", self)
        action.setShortcut(self.shortcut_copy.key())
        action.setToolTip("Copy selected values to the clipboard")
        action.setVisible(True)
        action.triggered.connect(self.copy)
        self.context_menu.addAction(action)

        action = QAction("Copy Including Headers", self)
        action.setShortcut(self.shortcut_copy_including_headers.key())
        action.setToolTip("Copy selected values and headers to the clipboard")
        action.setVisible(True)
        action.triggered.connect(self.copy_including_headers)
        self.context_menu.addAction(action)

        if self.allow_to_paste:
            action = QAction("Paste", self)
            action.setShortcut(self.shortcut_past.key())
            action.setToolTip("Paste values contained in the clipboard")
            action.setVisible(True)
            action.triggered.connect(self.paste)
            self.context_menu.addAction(action)

            action = QAction("Paste Special", self)
            action.setShortcut(self.shortcut_past_vars.key())
            action.setToolTip("Paste values according to variable names and optionally periods")
            action.setVisible(True)
            action.triggered.connect(self.paste_vars)
            self.context_menu.addAction(action)

    @Slot(QContextMenuEvent)
    def popup_context_menu(self, event: QContextMenuEvent):
        """
        Popup context menu at the event's global position.

        :param event: The context menu event.
        :type event: QContextMenuEvent
        """
        self.context_menu.popup(event.globalPos())

    def _extract_objects_names(self) -> List[str]:
        """
        Extract the names of the selected objects.

        :return: A list of the names of the selected objects.
        :rtype: list[str]
        """
        selection_range: QItemSelectionRange = self.selectionModel().selection().first()
        model: IodeAbstractTableModel = self.model()
        object_names = [model.headerData(row, Qt.Orientation.Vertical, Qt.ItemDataRole.DisplayRole) 
                        for row in range(selection_range.top(), selection_range.bottom() + 1)]
        return object_names

    def _extract_periods(self) -> List[str]:
        """
        Extract the periods of the selected data.

        :return: A list of the periods.
        :rtype: list[str]
        """
        selection_range: QItemSelectionRange = self.selectionModel().selection().first()
        model: IodeAbstractTableModel = self.model()
        periods = [model.headerData(column, Qt.Orientation.Horizontal, Qt.ItemDataRole.DisplayRole) 
                   for column in range(selection_range.left(), selection_range.right() + 1)]
        return periods

    def _extract_values(self) -> List[List[str]]:
        """
        Extract the values of the selected data.

        :return: A list of lists containing the values of the selected data.
        :rtype: list[list[str]]
        """
        selection_range: QItemSelectionRange = self.selectionModel().selection().first()
        model: IodeAbstractTableModel = self.model()
        values = []
        for row in range(selection_range.top(), selection_range.bottom() + 1):
            row_values = [model.data(model.index(row, column), Qt.ItemDataRole.DisplayRole) 
                          for column in range(selection_range.left(), selection_range.right() + 1)]
            row_values = ["" if value == NAN_REP else value for value in row_values]
            values += [row_values]
        return values

    def _ask_sample(self) -> Tuple[str, str]:
        """
        This method opens a dialog to ask for sample data.

        :return: A tuple containing the 'from' and 'to' values.
        :rtype: tuple
        """
        try:
            dialog = EditIodeSampleDialog(self)
            if dialog.exec() == QDialog.DialogCode.Accepted:
                return dialog.from_period, dialog.to_period
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def copy(self):
        """
        Copy the selected values to the clipboard.

        If an error occurs during copying, a warning message box is shown.
        """
        try:
            values: List[List[str]] = self._extract_values()
            text = '\n'.join(['\t'.join(row) for row in values])
            clipboard = QApplication.clipboard()
            clipboard.setText(text)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", "Can't copy values\n\n" + str(e))
    
    @Slot()
    def copy_including_headers(self):
        """
        Copy values including headers to clipboard.
        """
        try:
            objects_names: List[str] = self._extract_objects_names()
            periods: List[str] = self._extract_periods()
            values: List[List[str]] = self._extract_values()

            text = "\t" + "\t".join(periods) + "\n"
            for row in range(len(values)):
                text += objects_names[row] + "\t" + "\t".join(values[row]) + "\n"

            clipboard = QApplication.clipboard()
            clipboard.setText(text)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", "Can't copy values\n\n" + str(e))

    @Slot()
    def paste(self):
        """
        Paste values from clipboard.
        """
        if self.allow_to_paste:
            try:
                start_index: QModelIndex = self.selectionModel().selectedIndexes()[0]
                model: IodeAbstractTableModel = self.model()

                clipboard = QApplication.clipboard()
                text = clipboard.text()

                # If the text from clipboard ends with the new-line character,
                # the split() function returns a list ending by an empty element.
                if text.endswith("\n"):
                    text = text[:-1]

                for i, row_string in enumerate(text.split("\n")):
                    for j, value in enumerate(row_string.split("\t")):
                        index = model.index(start_index.row() + i, start_index.column() + j)
                        if index.isValid():
                            model.setData(index, value, Qt.ItemDataRole.EditRole)
            except Exception as e:
                QMessageBox.warning(None, "WARNING", "Can't paste values\n\n" + str(e))   

    @Slot()
    def paste_vars(self):
        """
        This method pastes variables from the clipboard into the model.
        """
        if self.allow_to_paste:
            try:
                model: IodeAbstractTableModel = self.model()

                clipboard = QGuiApplication.clipboard()
                text = clipboard.text()

                # If the text from clipboard ends with the new-line character,
                # the QString::split() function returns a list ending by an empty element.
                # This empty element is then interpreted as a new NaN value by the setData function.
                if text.endswith("\n"):
                    text = text[:-1]

                lines = text.split("\n")
                if len(lines) == 0:
                    return

                # ---- extract columns ----
                sample_vars = variables.sample

                # check if the first line represents the periods
                cells = lines[0].split("\t")
                if cells[0] == "":
                    from_period = cells[1]
                    to_period = cells[-1]
                    lines = lines[1:]
                else:
                    from_period, to_period = self._ask_sample()

                # check that the passed sample has the size equal to the number of cells per line.
                # We compare to len(cells) -1 because the first cell is supposed to contains
                # the variable name, not a value
                sample = Sample(from_period, to_period)
                if sample.nb_periods != len(cells) - 1:
                    QMessageBox.warning(None, "WARNING", "Can't paste values.\n" + 
                                        f"The passed sample {str(sample)} does not " + 
                                        f"correspond to number of values per variable: '{len(cells)}'")
                    return

                start_column = sample_vars.index(from_period)

                for line in lines:
                    cells = line.split("\t")

                    var_name = cells.pop(0).strip()
                    # raises an error if the the variable name is not found
                    row = variables.get_position(var_name)

                    column = start_column
                    for value in cells:
                        index = model.index(row, column)
                        if index.isValid():
                            model.setData(index, value, Qt.ItemDataRole.EditRole)
                        column += 1
            except Exception as e:
                QMessageBox.warning(None, "WARNING", f"Can't paste values\n\n{str(e)}")
