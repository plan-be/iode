from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from settings import MixinSettingsDialog
from .ui_workspace_copy_into import Ui_MenuWorkspaceCopyInto


from iode import (IodeType, IodeFileType, comments, equations, identities, 
                  lists, scalars, tables, variables)


class MenuWorkspaceCopyInto(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuWorkspaceCopyInto()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.v_files_types = list(IodeFileType)
        self.v_iode_types = list(IodeType)
        v_iode_type_names = [iode_type.name for iode_type in self.v_iode_types]
        self.ui.comboBox_object_type.addItems(v_iode_type_names)    
        self.ui.comboBox_object_type.currentIndexChanged.connect(self.update_file_chooser)

        self.load_settings()

    @Slot()
    def copy_into_workspace(self):
        try:
            iode_type: IodeType = self.v_iode_types[self.ui.comboBox_object_type.currentIndex()]
            input_file: str = self.ui.fileChooser_input_file.filepath
            from_period: str = self.ui.sampleEdit_sample_from.text()
            to_period: str = self.ui.sampleEdit_sample_to.text()
            object_names: str = ";".join(self.ui.textEdit_object_names.toPlainText().splitlines()).strip()

            if not len(input_file):
                raise ValueError("'input file' is empty")
            
            if not len(from_period):
                from_period = None
            if not len(to_period):
                to_period = None

            if iode_type == IodeType.COMMENTS:
                comments.copy_from(input_file, object_names)
            elif iode_type == IodeType.EQUATIONS:
                equations.copy_from(input_file, object_names)
            elif iode_type == IodeType.IDENTITIES:
                identities.copy_from(input_file, object_names)
            elif iode_type == IodeType.LISTS:
                lists.copy_from(input_file, object_names)
            elif iode_type == IodeType.SCALARS:
                scalars.copy_from(input_file, object_names)
            elif iode_type == IodeType.TABLES:
                tables.copy_from(input_file, object_names)
            elif iode_type == IodeType.VARIABLES:
                variables.copy_from(input_file, from_period, to_period, object_names)

            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "Failed to copy into workspace.\n" + str(e))

    @Slot(int)
    def update_file_chooser(self, i_iode_type: int):
        self.ui.fileChooser_input_file.enum_file_type = self.v_files_types[i_iode_type]
