from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from .ui_duplicate import Ui_MenuDataDuplicateObj

from copy import copy
from iode import (IodeType, comments, equations, identities, 
                  lists, scalars, tables, variables)


class MenuDataDuplicateObj(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuDataDuplicateObj()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.v_iode_types = list(IodeType)
        v_iode_type_names = [iode_type.name for iode_type in self.v_iode_types]
        self.ui.comboBox_iode_types.addItems(v_iode_type_names)
        self.ui.comboBox_iode_types.setCurrentIndex(0)    

        self.ui.lineEdit_obj_name.handle_iode_type(IodeType.COMMENTS)        
        self.ui.lineEdit_obj_name.include_iode_command(False)        
        self.ui.lineEdit_obj_name.include_lec_functions(False)

        self.ui.comboBox_iode_types.currentIndexChanged.connect(self.update_name_completer)
        self.load_settings()

    @Slot(int)
    def update_name_completer(self, i_iode_type):
        iode_type: IodeType = self.v_iode_types[i_iode_type]
        self.ui.lineEdit_obj_name.reset_iode_types()
        self.ui.lineEdit_obj_name.handle_iode_type(iode_type)

    @Slot()
    def duplicate(self):
        try:
            iode_type: IodeType = self.v_iode_types[self.ui.comboBox_iode_types.currentIndex()]
            obj_name: str = self.ui.lineEdit_obj_name.text().strip()
            dup_obj_name: str = self.ui.lineEdit_dup_obj_name.text().strip()

            if iode_type == IodeType.COMMENTS:
                comments[dup_obj_name] = comments[obj_name]
            elif iode_type == IodeType.EQUATIONS:
                raise RuntimeError("Cannot duplicate an equation")
            elif iode_type == IodeType.IDENTITIES:
                identities[dup_obj_name] = copy(identities[obj_name])
            elif iode_type == IodeType.LISTS:
                lists[dup_obj_name] = copy(lists[obj_name])
            elif iode_type == IodeType.SCALARS:
                scalars[dup_obj_name] = copy(scalars[obj_name])
            elif iode_type == IodeType.TABLES:
                tables[dup_obj_name] = copy(tables[obj_name])
            elif iode_type == IodeType.VARIABLES:
                variables[dup_obj_name] = copy(variables[obj_name])

            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", f"Failed to duplicate IODE {iode_type.name} "
                                f"'{obj_name}'.\n" + str(e))
