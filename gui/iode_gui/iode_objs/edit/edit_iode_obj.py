from qtpy.QtCore import Qt, Signal, Slot
from qtpy.QtWidgets import QDialog, QPlainTextEdit, QMessageBox
from qtpy.QtGui import QDesktopServices, QShortcut, QKeySequence


from iode_gui.utils import URL_MANUAL
from iode_gui.text_edit.completer import IodeCompleter
from .ui_edit_iode_obj import Ui_EditObjectDialog

from typing import Any
from iode import IodeType, IODE_DATABASE_TYPE_NAMES, comments, identities, lists


class AbstractEditObjDialog(QDialog):
    database_modified = Signal()

    def __init__(self, name: str, database, global_database, parent=None):
        super().__init__(parent)
        self.previous_name = name
        self._database = database
        self._global_database = global_database
        self.previous_value = None

        iode_type = database.iode_type
        
        self.setWindowFlags(Qt.WindowType.Window)
        self.setWindowTitle("Editing " + IODE_DATABASE_TYPE_NAMES[int(iode_type)] + " " + name)

        self.full_screen_shortcut = QShortcut(QKeySequence(Qt.Modifier.CTRL | Qt.Key.Key_X), self)
        self.full_screen_shortcut.setContext(Qt.ShortcutContext.WidgetWithChildrenShortcut)
        self.full_screen_shortcut.activated.connect(self.toggle_maximize)

    def _edit(self, name: str, value: Any):
        try:
            # user changed nothing
            if name == self.previous_name and value == self.previous_value:
                self.accept()
                return

            # user changed the name when editing the object but another object with the same name already 
            # exists in the global database
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

    @Slot()
    def toggle_maximize(self):
        """Toggles full screen mode."""
        if self.isMaximized():
            self.showNormal()
        else:
            self.showMaximized()


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

        self.ui.textEdit_content.setup_completer(iode_types=[IodeType.SCALARS, IodeType.VARIABLES])

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

        self.ui.textEdit_content.setup_completer(iode_types=[IodeType.SCALARS, IodeType.VARIABLES])

    @Slot()
    def edit(self):
        name = self.ui.lineEdit_name.text()
        value = self.ui.textEdit_content.toPlainText()
        self._edit(name, value)


