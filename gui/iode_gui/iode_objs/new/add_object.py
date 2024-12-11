from PySide6.QtCore import Signal, Slot
from PySide6.QtWidgets import QDialog, QMessageBox
from PySide6.QtGui import QDesktopServices, QDoubleValidator

from text_edit.completer import IodeCompleter
from .ui_add_object import Ui_AddObjectDialog
from .ui_add_scalar import Ui_AddScalarDialog
from .ui_add_table import Ui_AddTableDialog

from iode_gui.utils import URL_MANUAL
from iode import (IodeType, Table, comments, identities, lists, scalars, 
                  tables, variables)


class AbstractAddDialog(QDialog):
    new_object_inserted = Signal(str)

    def __init__(self, database, global_database, parent=None):
        super().__init__(parent)
        self._database = database
        self._global_database = global_database

    def _add(self, name: str, value):
        try:
            name = name.strip()

            # check if already exists in the global IODE database since the attribute '_database' 
            # may represents only a subset
            if name in self._global_database:
                res = QMessageBox.question(None, "WARNING", f"{name} already exists. Replace it ?")
                if res != QMessageBox.StandardButton.Yes:
                    self.reject()
                    return
            
            self._database[name] = value
            self.accept()
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))

    @Slot()
    def add(self):
        """
        Add a new IODE object to the database
        """
        NotImplementedError()

    @Slot()
    def help(self):
        """
        Open the help manual
        """
        QDesktopServices.openUrl(URL_MANUAL)


class AddCommentDialog(AbstractAddDialog):
    def __init__(self, database, parent=None):
        super().__init__(database, comments, parent)
        assert database.iode_type == IodeType.COMMENTS
        self.ui = Ui_AddObjectDialog()
        self.ui.setupUi(self)

    @Slot()
    def add(self):
        name = self.ui.lineEdit_name.text()
        value = self.ui.lineEdit_definition.text()
        self._add(name, value)


class AddIdentityDialog(AbstractAddDialog):
    def __init__(self, database, parent=None):
        super().__init__(database, identities, parent)
        assert database.iode_type == IodeType.IDENTITIES
        self.ui = Ui_AddObjectDialog()
        self.ui.setupUi(self)

    @Slot()
    def add(self):
        name = self.ui.lineEdit_name.text()
        value = self.ui.lineEdit_definition.text()
        self._add(name, value)


class AddListDialog(AbstractAddDialog):
    def __init__(self, database, parent=None):
        super().__init__(database, lists, parent)
        assert database.iode_type == IodeType.LISTS
        self.ui = Ui_AddObjectDialog()
        self.ui.setupUi(self)

    @Slot()
    def add(self):
        name = self.ui.lineEdit_name.text()
        value = self.ui.lineEdit_definition.text()
        self._add(name, value)


class AddScalarDialog(AbstractAddDialog):
    def __init__(self, database, parent=None):
        super().__init__(database, scalars, parent)
        assert database.iode_type == IodeType.SCALARS
        self.ui = Ui_AddScalarDialog()
        self.ui.setupUi(self)
        self.ui.lineEdit_value.setValidator(QDoubleValidator(parent));

    @Slot()
    def add(self):
        name = self.ui.lineEdit_name.text()
        value = float(self.ui.lineEdit_value.text())
        relax = self.ui.doubleSpinBox_relax.value()
        self._add(name, (value, relax))


class AddTableDialog(AbstractAddDialog):
    def __init__(self, database, parent=None):
        super().__init__(database, tables, parent)
        assert database.iode_type == IodeType.TABLES
        self.ui = Ui_AddTableDialog()
        self.ui.setupUi(self)
        completer = IodeCompleter(report_commands=False, lec_functions=False, 
                                  iode_types=[IodeType.SCALARS, IodeType.VARIABLES])
        self.ui.textEdit_lec.completer = completer

    @Slot()
    def add(self):
        name = self.ui.lineEdit_name.text()
        nb_columns = self.ui.spinBox_nb_columns.value()
        title = self.ui.lineEdit_table_title.text()
        lecs = self.ui.textEdit_lec.text()
        mode = self.ui.checkBox_mode.isChecked()
        files = self.ui.checkBox_files.isChecked()
        date = self.ui.checkBox_date.isChecked()

        table: Table = Table(nb_columns, title, lecs, mode, files, date)
        self._add(name, table)


class AddVariableDialog(AbstractAddDialog):
    def __init__(self, database, parent=None):
        super().__init__(database, variables, parent)
        assert database.iode_type == IodeType.VARIABLES
        self.ui = Ui_AddObjectDialog()
        self.ui.setupUi(self)

    @Slot()
    def add(self):
        name = self.ui.lineEdit_name.text()
        lec = self.ui.lineEdit_definition.text()
        self._add(name, lec)
