from qtpy.QtCore import Slot
from qtpy.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from iode_gui.abstract_main_window import AbstractMainWindow
from .ui_workspace_merge_into import Ui_MenuWorkspaceMergeInto


from iode import (IodeType, IodeFileType, comments, equations, identities, 
                  lists, scalars, tables, variables)


class MenuWorkspaceMergeInto(MixinSettingsDialog):
    def __init__(self, parent: AbstractMainWindow):
        super().__init__(parent)
        self.ui = Ui_MenuWorkspaceMergeInto()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.v_files_types = list(IodeFileType)
        self.v_iode_types = list(IodeType)
        v_iode_type_names = [iode_type.name for iode_type in self.v_iode_types]
        self.ui.comboBox_object_type.addItems(v_iode_type_names)    
        self.ui.comboBox_object_type.currentIndexChanged.connect(self.update_file_chooser)

        self.load_settings()

    @Slot()
    def merge_into_workspace(self):
        try:
            iode_type: IodeType = self.v_iode_types[self.ui.comboBox_object_type.currentIndex()]
            input_file: str = self.ui.fileChooser_input_file.filepath

            if not len(input_file):
                raise ValueError("'input file' is empty")

            if iode_type == IodeType.COMMENTS:
                comments.merge_from(input_file)
            elif iode_type == IodeType.EQUATIONS:
                equations.merge_from(input_file)
            elif iode_type == IodeType.IDENTITIES:
                identities.merge_from(input_file)
            elif iode_type == IodeType.LISTS:
                lists.merge_from(input_file)
            elif iode_type == IodeType.SCALARS:
                scalars.merge_from(input_file)
            elif iode_type == IodeType.TABLES:
                tables.merge_from(input_file)
            elif iode_type == IodeType.VARIABLES:
                variables.merge_from(input_file)

            self.accept()
        except Exception as e:
            self.parent().display_output(f"ERROR -> {str(e)}")
            QMessageBox.warning(self, "WARNING", "Failed to merge into workspace:\n" + str(e))

    @Slot(int)
    def update_file_chooser(self, i_iode_type: int):
        self.ui.fileChooser_input_file.enum_file_type = self.v_files_types[i_iode_type]
