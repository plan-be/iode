from PySide6.QtCore import Qt, Slot, QModelIndex, QItemSelectionRange
from PySide6.QtWidgets import QMenu, QApplication, QMessageBox, QDialog
from PySide6.QtGui import (QKeySequence, QAction, QShortcut, QContextMenuEvent, 
                           QClipboard, QGuiApplication)

from iode_gui.utils import NAN_REP
from iode_gui.iode_objs.edit.edit_vars_sample import EditIodeSampleDialog
from iode_gui.iode_objs.models.abstract_table_model import IodeAbstractTableModel
from iode_gui.iode_objs.models.numerical_table_model import IodeNumericalTableModel

import re
from typing import List, Tuple


_name_pattern = re.compile(r"^[a-zA-Z_][a-zA-Z0-9_]*$")


class NumericalTableView:
    def __init__(self):
        self.allow_to_paste: bool = None
        self.context_menu: QMenu = None

    # NOTE: need a setup() method since NumericalTableView.__init__() seems to be called automatically inside 
    #       super().__init__(parent) (i.e. called inside QTableView.__init__(parent)) in the beginning of 
    #       IodeAbstractTableView.__init__()
    def setup(self, allow_to_paste: bool):
        self.allow_to_paste = allow_to_paste
        
        self.shortcut_copy = QShortcut(QKeySequence.StandardKey.Copy, self)
        self.shortcut_copy.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.shortcut_copy.activated.connect(self.copy)

        self.shortcut_copy_including_headers = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.SHIFT | Qt.Key.Key_C), self)
        self.shortcut_copy_including_headers.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.shortcut_copy_including_headers.activated.connect(self.copy_including_headers)

        if allow_to_paste:
            self.shortcut_past = QShortcut(QKeySequence.StandardKey.Paste, self)
            self.shortcut_past.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
            self.shortcut_past.activated.connect(self.paste)

            self.shortcut_past_vars = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Modifier.SHIFT | Qt.Key.Key_V), self)
            self.shortcut_past_vars.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
            self.shortcut_past_vars.activated.connect(self.paste_special)

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
            action.triggered.connect(self.paste_special)
            self.context_menu.addAction(action)

    @Slot(QContextMenuEvent)
    def popup_context_menu(self, event: QContextMenuEvent):
        """
        Popup context menu at the event's global position.

        :param event: The context menu event.
        :type event: QContextMenuEvent
        """
        self.context_menu.popup(event.globalPos())

    # NOTE: Weirdly, we have to store values of the selection borders into first/last_row/column 
    #       variables to get the real values of the selection borders. 
    #       If we try range(selection_range.top(), selection_range.bottom() + 1), 
    #       we get wrong values for the borders (like -1).
    def _extract_objects_names(self) -> List[str]:
        """
        Extract the names of the selected objects.

        :return: A list of the names of the selected objects.
        :rtype: list[str]
        """
        selection_range: QItemSelectionRange = self.selectionModel().selection().first()
        first_row = selection_range.top()
        last_row = selection_range.bottom()
        if first_row == -1 or last_row == -1:
            QMessageBox.warning(self, "WARNING", "Selection is empty")
            return [[]]
        model = self.model()
        object_names = [model.headerData(row, Qt.Orientation.Vertical, Qt.ItemDataRole.DisplayRole) 
                        for row in range(first_row, last_row + 1)]
        if not all(isinstance(name, str) for name in object_names):
            raise RuntimeError("Something went wrong while extracting row names")
        return object_names

    # NOTE: Weirdly, we have to store values of the selection borders into first/last_row/column 
    #       variables to get the real values of the selection borders. 
    #       If we try range(selection_range.left(), selection_range.right() + 1), 
    #       we get wrong values for the borders (like -1).
    def _extract_periods(self) -> List[str]:
        """
        Extract the periods of the selected data.

        :return: A list of the periods.
        :rtype: list[str]
        """
        selection_range: QItemSelectionRange = self.selectionModel().selection().first()
        first_column = selection_range.left()
        last_column = selection_range.right()
        if first_column == -1 or last_column == -1:
            QMessageBox.warning(self, "WARNING", "Selection is empty")
            return [[]]
        model = self.model()
        periods = [model.headerData(column, Qt.Orientation.Horizontal, Qt.ItemDataRole.DisplayRole) 
                   for column in range(first_column, last_column + 1)]
        if not all(isinstance(period, str) for period in periods):
            raise RuntimeError("Something went wrong while extracting periods")
        return periods
    # NOTE: Weirdly, we have to store values of the selection borders into first/last_row/column 
    #       variables to get the real values of the selection borders. 
    #       If we try range(selection_range.top(), selection_range.bottom() + 1) or 
    #       range(selection_range.left(), selection_range.right() + 1), we get wrong values
    #       for the borders (like -1).
    def _extract_values(self) -> List[List[str]]:
        """
        Extract the values of the selected data.

        :return: A list of lists containing the values of the selected data.
        :rtype: list[list[str]]
        """
        selection_range: QItemSelectionRange = self.selectionModel().selection().first()
        first_row = selection_range.top()
        last_row = selection_range.bottom()
        first_column = selection_range.left()
        last_column = selection_range.right()
        if first_row == -1 or last_row == -1 or first_column == -1 or last_column == -1:
            QMessageBox.warning(self, "WARNING", "Selection is empty")
            return [[]]
        model = self.model()
        values = []
        for row in range(first_row, last_row + 1):
            row_values = [model.data(model.index(row, column), Qt.ItemDataRole.DisplayRole) for column in 
                          range(first_column, last_column + 1)]
            if not all(isinstance(value, str) for value in row_values):
                raise RuntimeError("Something went wrong while extracting values")
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
                # the split() function in the loop below will return a list 
                # ending by an empty element.
                if text.endswith("\n"):
                    text = text[:-1]

                ok = True
                for i, row_string in enumerate(text.split("\n")):
                    for j, value in enumerate(row_string.split("\t")):
                        row = start_index.row() + i
                        column = start_index.column() + j
                        index = model.index(row, column)
                        if index.isValid():
                            ok = model.setData(index, value, Qt.EditRole)
                            if not ok:
                                return
                        else:
                            raise IndexError(f"Invalid index at row {row} and column {column}")
            except Exception as e:
                QMessageBox.warning(None, "WARNING", "Can't continue to paste values\n\n" + str(e))   

    def _paste_special(self, names: List[str], columns: List[str], values: List[List[float]]):
        raise NotImplementedError()

    @Slot()
    def paste_special(self):
        """
        Paste variables from clipboard. 
        The content of the clipboard must include the variable names.
        Optionally, it can include the periods.
        """
        if self.allow_to_paste:
            try:
                clipboard = QGuiApplication.clipboard()
                text = clipboard.text()
                _model: IodeNumericalTableModel = self.model()

                # If the text from clipboard ends with the new-line character, the 
                # split() function below will return a list ending by an empty element.
                if text.endswith("\n"):
                    text = text[:-1]

                lines = text.split("\n")
                if len(lines) == 0:
                    return

                # ---- extract columns names ----
                columns: List[str] = lines.pop(0).split("\t")
                columns = [cell.strip() for cell in columns[1:]]

                # ---- extract rows (= names) and values ----   
                names: List[str] = []
                values: List[List[float]] = []
                for i, line in enumerate(lines):
                    cells = line.split("\t")
                    name = cells.pop(0).strip()
                    if not name:
                        raise ValueError(f"Empty name at row {i}")
                    if not _name_pattern.fullmatch(name):
                        raise ValueError(f"Invalid name '{name}' at row {i}")
                    names.append(name)

                    cells = [_model.string_to_float(cell.strip()) for cell in cells]
                    values.append(cells)
                
                self._paste_special(names, columns, values)      
            except Exception as e:
                QMessageBox.warning(None, "WARNING", f"Can't paste values\n\n{str(e)}")
