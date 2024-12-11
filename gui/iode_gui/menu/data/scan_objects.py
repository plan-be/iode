from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from .ui_scan_objects import Ui_MenuDataScanObjects

from iode import IodeType, equations, identities, lists, tables


class MenuDataScanObjects(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuDataScanObjects()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.v_iode_types = [IodeType.EQUATIONS, IodeType.IDENTITIES, IodeType.TABLES]
        v_iode_type_names = [iode_type.name for iode_type in self.v_iode_types]
        self.ui.comboBox_iode_types.addItems(v_iode_type_names)
        self.ui.comboBox_iode_types.setCurrentIndex(0)
        self.update_name_completer(0)

        self.ui.textEdit_lists_to_scan.handle_iode_type(IodeType.COMMENTS)        
        self.ui.textEdit_lists_to_scan.include_iode_command(False)        
        self.ui.textEdit_lists_to_scan.include_lec_functions(False)

        self.ui.comboBox_iode_types.currentIndexChanged.connect(self.update_name_completer)
        self.load_settings()

    @Slot(int)
    def update_name_completer(self, i_iode_type):
        iode_type: IodeType = self.v_iode_types[i_iode_type]
        self.ui.textEdit_lists_to_scan.reset_iode_types()
        self.ui.textEdit_lists_to_scan.handle_iode_type(iode_type)

    @Slot()
    def scan(self):
        try:
            i_iode_type = self.ui.comboBox_iode_types.currentIndex()
            iode_type: IodeType = self.v_iode_types[i_iode_type]
            objects_to_scan: str = self.ui.textEdit_lists_to_scan.toPlainText().strip()
            
            if not objects_to_scan:
                raise ValueError(f"No {iode_type.name} to be scanned")
            
            if iode_type == IodeType.EQUATIONS:
                coeffs_list = equations[objects_to_scan].coefficients
                vars_list = equations[objects_to_scan].variables
            elif iode_type == IodeType.IDENTITIES:
                coeffs_list = identities[objects_to_scan].coefficients
                vars_list = identities[objects_to_scan].variables 
            elif iode_type == IodeType.TABLES:
                coeffs_list = tables[objects_to_scan].coefficients
                vars_list = tables[objects_to_scan].variables
            
            lists["_SCAL"] = coeffs_list
            lists["_EXO"] = vars_list

            result = f"scalars (_SCAL list):   {'; '.join(coeffs_list)}\n\n"
            result += f"exogenous (_EXO list): {'; '.join(vars_list)}"
            self.ui.textEdit_note.setPlainText(result)
        except Exception as e:
            QMessageBox.warning(self, "WARNING", str(e))
