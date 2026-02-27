from qtpy.QtCore import Slot
from qtpy.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from iode_gui.text_edit.completer import IodeCompleter
from iode_gui.abstract_main_window import AbstractMainWindow
from .ui_compute_identities import Ui_MenuComputeIdentities

from typing import Union, List
from iode import IodeType, TableLang, IodeFileType, Sample, identities, variables


class MenuComputeIdentities(MixinSettingsDialog):
    def __init__(self, parent: AbstractMainWindow):
        super().__init__(parent)
        self.ui = Ui_MenuComputeIdentities()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.v_table_langs = list(TableLang)
        v_table_lang_names = [table_lang.name.title() for table_lang in self.v_table_langs]
        self.ui.comboBox_language.addItems(v_table_lang_names)
        self.ui.comboBox_language.setCurrentIndex(0)   

        self.ui.textEdit_identities_list.setup_completer(iode_types=IodeType.IDENTITIES)

        self.ui.fileChooser_var_file1.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_var_file2.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_var_file3.enum_file_type = IodeFileType.FILE_VARIABLES

        self.ui.fileChooser_scl_file1.enum_file_type = IodeFileType.FILE_SCALARS
        self.ui.fileChooser_scl_file2.enum_file_type = IodeFileType.FILE_SCALARS
        self.ui.fileChooser_scl_file3.enum_file_type = IodeFileType.FILE_SCALARS

        self.load_settings()

        vars_sample: Sample = variables.sample
        if vars_sample.nb_periods:
            from_period = str(vars_sample.start)
            to_period = str(vars_sample.end)
            if not self.ui.sampleEdit_sample_from.text():
                self.ui.sampleEdit_sample_from.setText(from_period)
            if not self.ui.sampleEdit_sample_to.text():
                self.ui.sampleEdit_sample_to.setText(to_period)

    @property
    def identities_names(self) -> List[str]:
        names: str = self.ui.textEdit_identities_list.toPlainText().strip()
        names = names.split(";")
        names = [name.strip() for name in names if name.strip()]
        return names

    @identities_names.setter
    def identities_names(self, names: Union[str, List[str]]):
        if not isinstance(names, str):
            names = "; ".join(names)
        self.ui.textEdit_identities_list.setPlainText(names)

    @Slot()
    def compute(self):
        try:
            from_period: str = self.ui.sampleEdit_sample_from.text().strip()
            to_period: str = self.ui.sampleEdit_sample_to.text().strip()
            # throw an error if from_period and/or to_period are/is invalid
            Sample(from_period, to_period)

            identities_names: str = self.ui.textEdit_identities_list.toPlainText().strip()

            var_files: List[str] = []
            var_files.append(self.ui.fileChooser_var_file1.filepath)
            var_files.append(self.ui.fileChooser_var_file2.filepath)
            var_files.append(self.ui.fileChooser_var_file3.filepath)
            var_files = [var_file for var_file in var_files if var_file]
            var_files = "; ".join(var_files) if len(var_files) else None

            scalar_files: List[str] = []
            scalar_files.append(self.ui.fileChooser_scl_file1.filepath)
            scalar_files.append(self.ui.fileChooser_scl_file2.filepath)
            scalar_files.append(self.ui.fileChooser_scl_file3.filepath)
            scalar_files = [scalar_file for scalar_file in scalar_files if scalar_file]
            scalar_files = "; ".join(scalar_files) if len(scalar_files) else None

            trace: bool = self.ui.checkBox_trace.isChecked()

            i_table_lang = self.ui.comboBox_language.currentIndex()
            table_lang: TableLang = self.v_table_langs[i_table_lang]

            identities.execute(identities_names, from_period, to_period, var_files, scalar_files, trace)
            
            self.accept()
        except Exception as e:
            self.parent().display_output(f"ERROR -> {str(e)}")
            QMessageBox.warning(self, "WARNING", str(e))
