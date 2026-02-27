
from qtpy.QtCore import Slot
from qtpy.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from iode_gui.abstract_main_window import AbstractMainWindow
from .ui_file_contents import Ui_MenuDataFileContents

from iode import (IodeType, IodeFileType, comments, equations, identities, 
                  lists, scalars, tables, variables)


class MenuDataFileContents(MixinSettingsDialog):
    def __init__(self, parent: AbstractMainWindow):
        super().__init__(parent)
        self.ui = Ui_MenuDataFileContents()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.v_iode_types = list(IodeType)
        v_iode_type_names = [iode_type.name for iode_type in self.v_iode_types]
        self.ui.comboBox_iode_types.addItems(v_iode_type_names)
        self.ui.comboBox_iode_types.setCurrentIndex(0)
        self.update_file_chooser(0)

        self.ui.lineEdit_search.setPlaceholderText("search pattern: A*;*_")
        self.ui.lineEdit_search.setText("_RES")

        self.ui.comboBox_iode_types.currentIndexChanged.connect(self.update_file_chooser)

        self.load_settings()

    @Slot()
    def search(self):
        try:
            pattern: str = self.ui.lineEdit_search.text()
            i_iode_type = self.ui.comboBox_iode_types.currentIndex()
            iode_type: IodeType = self.v_iode_types[i_iode_type]
            input_file: str = self.ui.fileChooser_input_file.filepath
            save_list_name: str = self.ui.lineEdit_save_list.text()

            if pattern == "":
                raise Exception("No search pattern provided.")
            
            if input_file == "":
                input_file = None
            
            if iode_type == IodeType.COMMENTS:
                result_list = comments.get_names(pattern, input_file)
            elif iode_type == IodeType.EQUATIONS:
                result_list = equations.get_names(pattern, input_file)
            elif iode_type == IodeType.IDENTITIES:
                result_list = identities.get_names(pattern, input_file)
            elif iode_type == IodeType.LISTS:
                result_list = lists.get_names(pattern, input_file)
            elif iode_type == IodeType.SCALARS:
                result_list = scalars.get_names(pattern, input_file)
            elif iode_type == IodeType.TABLES:
                result_list = tables.get_names(pattern, input_file)
            elif iode_type == IodeType.VARIABLES:
                result_list = variables.get_names(pattern, input_file)

            if save_list_name:
                lists[save_list_name] = result_list

            self.ui.label_nb_elements.setText(f"{len(result_list)} Elements")
            self.ui.textEdit_result.setText(";".join(result_list))

        except Exception as e:
            self.parent().display_output(f"ERROR -> {str(e)}")
            QMessageBox.warning(self, "WARNING", str(e))

    @Slot(int)
    def update_file_chooser(self, i_iode_type: int):
        iode_type: IodeType = self.v_iode_types[i_iode_type]
        file_type: IodeFileType = IodeFileType(int(iode_type))
        self.ui.fileChooser_input_file.enum_file_type = file_type
