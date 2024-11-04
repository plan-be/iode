from PySide6.QtCore import Qt, Signal, Slot, QModelIndex
from PySide6.QtWidgets import QDialog, QPlainTextEdit, QMessageBox
from PySide6.QtGui import QDesktopServices, QShortcut, QKeySequence


from utils import URL_MANUAL
from abstract_main_window import AbstractMainWindow
from text_edit.completer import IodeCompleter
from .ui_edit_iode_obj import Ui_EditObjectDialog

from typing import Any
from enum import Enum
from iode import (IodeTypes, IODE_DATABASE_TYPE_NAMES, comments, identities, lists, 
                  scalars, tables, variables, Table, TableLineType)


class AbstractEditObjDialog(QDialog):
    database_modified = Signal()

    def __init__(self, name: str, database, global_database, parent=None):
        super().__init__(parent)
        self.previous_name = name
        self._database = database
        self._global_database = global_database
        self.previous_value = None

        iode_type = database.iode_type
        self.setWindowTitle("Editing " + IODE_DATABASE_TYPE_NAMES[int(iode_type)] + " " + name)

        self.full_screen_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_X), self)
        self.full_screen_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.full_screen_shortcut.activated.connect(self.showMaximized)

    def _edit(self, name: str, value: Any):
        try:
            # user changed nothing
            if name == self.previous_name and value == self.previous_value:
                self.accept()
                return

            # user changed the name when editing but an IODE object with the same name already exist 
            # in the global database
            if name != self.previous_name and name in self._global_database:
                res = QMessageBox.question(None, "WARNING", f"{name} already exists. Replace it ?", 
                                           QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No)
                if res != QMessageBox.StandardButton.Yes:
                    self.reject()
                    return

            self._database[name] = value
            self.database_modified.emit()
            self.accept()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def edit(self):
        raise NotImplementedError()

    @Slot()
    def help(self):
        """
        Opens the manual in a web browser.
        """
        QDesktopServices.openUrl(URL_MANUAL)


class EditCommentDialog(AbstractEditObjDialog):
    def __init__(self, name: str, database, parent=None):
        super().__init__(name, database, comments, parent)
        self.ui = Ui_EditObjectDialog()
        self.ui.setupUi(self)

        self.ui.lineEdit_name.setText(name)
        self.ui.lineEdit_name.setReadOnly(True)

        self.previous_value = database[name]
        self.ui.textEdit_content.setLineWrapMode(QPlainTextEdit.LineWrapMode.WidgetWidth)
        self.ui.textEdit_content.setPlainText(self.previous_value)

    @Slot()
    def edit(self):
        name = self.ui.lineEdit_name.text()
        value = self.ui.textEdit_content.toPlainText()
        self._edit(name, value)


class EditIdentityDialog(AbstractEditObjDialog):
    def __init__(self, name: str, database, parent=None):
        super().__init__(name, database, identities, parent)
        self.ui = Ui_EditObjectDialog()
        self.ui.setupUi(self)

        self.ui.lineEdit_name.setText(name)
        self.ui.lineEdit_name.setReadOnly(True)

        self.previous_value = str(database[name])
        self.ui.textEdit_content.setLineWrapMode(QPlainTextEdit.LineWrapMode.WidgetWidth)
        self.ui.textEdit_content.setPlainText(self.previous_value)

        completer = IodeCompleter(report_commands=False, lec_functions=False, 
                                  iode_types=[IodeTypes.SCALARS, IodeTypes.VARIABLES])
        self.ui.textEdit_content.completer = completer

    @Slot()
    def edit(self):
        name = self.ui.lineEdit_name.text()
        value = self.ui.textEdit_content.toPlainText()
        self._edit(name, value)


class EditListDialog(AbstractEditObjDialog):
    def __init__(self, name: str, database, parent=None):
        super().__init__(name, database, lists, parent)
        self.ui = Ui_EditObjectDialog()
        self.ui.setupUi(self)

        self.ui.lineEdit_name.setText(name)
        self.ui.lineEdit_name.setReadOnly(True)

        self.previous_value = ", ".join(database[name])
        self.ui.textEdit_content.setLineWrapMode(QPlainTextEdit.LineWrapMode.WidgetWidth)
        self.ui.textEdit_content.setPlainText(self.previous_value)

        completer = IodeCompleter(report_commands=False, lec_functions=False, 
                                  iode_types=[IodeTypes.SCALARS, IodeTypes.VARIABLES])
        self.ui.textEdit_content.completer = completer

    @Slot()
    def edit(self):
        name = self.ui.lineEdit_name.text()
        value = self.ui.textEdit_content.toPlainText()
        self._edit(name, value)


