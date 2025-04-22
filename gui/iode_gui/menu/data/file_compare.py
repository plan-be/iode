from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox
from PySide6.QtGui import QDoubleValidator

from iode_gui.settings import MixinSettingsDialog
from iode_gui.abstract_main_window import AbstractMainWindow
from .ui_file_compare import Ui_MenuDataFileCompare

from iode import (IodeType, IodeFileType, comments, equations, identities, 
                  lists, scalars, tables, variables)


class MenuDataFileCompare(MixinSettingsDialog):
    def __init__(self, parent: AbstractMainWindow):
        super().__init__(parent)
        self.ui = Ui_MenuDataFileCompare()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.v_iode_types = list(IodeType)
        v_iode_type_names = [iode_type.name for iode_type in self.v_iode_types]
        self.ui.comboBox_iode_types.addItems(v_iode_type_names)
        self.ui.comboBox_iode_types.setCurrentIndex(0)   

        self.ui.lineEdit_threshold.setValidator(QDoubleValidator(self))
        self.ui.lineEdit_list_ws_only.setText("OLD")
        self.ui.lineEdit_list_file_only.setText("NEW")
        self.ui.lineEdit_list_both_equal.setText("SAME")
        self.ui.lineEdit_list_both_different.setText("CHANGED")
        self.maybe_enable_threshold_field(IodeType.COMMENTS)

        self.ui.comboBox_iode_types.currentIndexChanged.connect(self.maybe_enable_threshold_field)
        self.ui.comboBox_iode_types.currentIndexChanged.connect(self.update_file_chooser)

        self.load_settings()

    @Slot()
    def compare(self):
        try:
            i_iode_type = self.ui.comboBox_iode_types.currentIndex()
            iode_type: IodeType = self.v_iode_types[i_iode_type]
            threshold: float = float(self.ui.lineEdit_threshold.text())
            input_file: str = self.ui.fileChooser_input_file.filepath
            list_ws_only = self.ui.lineEdit_list_ws_only.text()
            list_file_only = self.ui.lineEdit_list_file_only.text()
            list_both_equal = self.ui.lineEdit_list_both_equal.text()
            list_different = self.ui.lineEdit_list_both_different.text()

            if not list_ws_only:
                raise ValueError(f"List name for '{self.ui.label_list_ws_only}' is empty.")
            if not list_file_only:
                raise ValueError(f"List name for '{self.ui.label_list_file_only}' is empty.")
            if not list_both_equal:
                raise ValueError(f"List name for '{self.ui.label_list_both_equal}' is empty.")
            if not list_different:
                raise ValueError(f"List name for '{self.ui.label_list_both_different}' is empty.")

            if iode_type == IodeType.VARIABLES:
                variables.threshold = threshold

            if iode_type == IodeType.COMMENTS:
                results = comments.compare(input_file, list_ws_only, list_file_only, list_both_equal, list_different)
            elif iode_type == IodeType.EQUATIONS:
                results = equations.compare(input_file, list_ws_only, list_file_only, list_both_equal, list_different)
            elif iode_type == IodeType.IDENTITIES:
                results = identities.compare(input_file, list_ws_only, list_file_only, list_both_equal, list_different)
            elif iode_type == IodeType.LISTS:
                results = lists.compare(input_file, list_ws_only, list_file_only, list_both_equal, list_different)
            elif iode_type == IodeType.SCALARS:
                results = scalars.compare(input_file, list_ws_only, list_file_only, list_both_equal, list_different)
            elif iode_type == IodeType.TABLES:
                results = tables.compare(input_file, list_ws_only, list_file_only, list_both_equal, list_different)
            elif iode_type == IodeType.VARIABLES:
                results = variables.compare(input_file, list_ws_only, list_file_only, list_both_equal, list_different)

            for name, value in results.items():
                lists[name] = value

            self.ui.label_nb_elements_ws_only.setText(f"{len(results[list_ws_only])} Elements")
            self.ui.label_nb_elements_file_only.setText(f"{len(results[list_file_only])} Elements")
            self.ui.label_nb_elements_both_equal.setText(f"{len(results[list_both_equal])} Elements")
            self.ui.label_nb_elements_both_different.setText(f"{len(results[list_different])} Elements")
        except Exception as e:
            self.parent().display_output(f"ERROR -> {str(e)}")
            QMessageBox.warning(self, "WARNING", f"Failed to compare {iode_type.name} with those "
                                f"contained in file '{input_file}'.\n" + str(e))

    @Slot(int)
    def update_file_chooser(self, i_iode_type: int):
        iode_type: IodeType = self.v_iode_types[i_iode_type]
        file_type: IodeFileType = IodeFileType(int(iode_type))
        self.ui.fileChooser_input_file.enum_file_type = file_type

    @Slot(int)
    def maybe_enable_threshold_field(self, i_iode_type: int):
        iode_type: IodeType = self.v_iode_types[i_iode_type]
        if iode_type == IodeType.VARIABLES:
            self.ui.lineEdit_threshold.setEnabled(True)
        else:
            self.ui.lineEdit_threshold.setEnabled(False)
