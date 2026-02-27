from qtpy.QtCore import Slot
from qtpy.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from iode_gui.util.widgets.file_chooser import EnumFileMode
from iode_gui.abstract_main_window import AbstractMainWindow
from .ui_file_import_variables import Ui_MenuFileImportVariables

from iode import IodeFileType, ImportFormats, Sample, variables


class MenuFileImportVariables(MixinSettingsDialog):
    def __init__(self, parent: AbstractMainWindow):
        super().__init__(parent)
        self.ui = Ui_MenuFileImportVariables()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.v_import_formats = list(ImportFormats)
        v_import_formats_names = [import_format.name.title() for import_format in self.v_import_formats]
        self.ui.comboBox_format.addItems(v_import_formats_names)
        self.ui.comboBox_format.setCurrentIndex(0)

        self.ui.fileChooser_input_file.enum_file_type = IodeFileType.FILE_ANY
        self.ui.fileChooser_input_file.enum_file_mode = EnumFileMode.EXISTING_FILE

        self.ui.fileChooser_rule_file.enum_file_type = IodeFileType.FILE_ANY
        self.ui.fileChooser_rule_file.enum_file_mode = EnumFileMode.EXISTING_FILE

        self.ui.fileChooser_save_file.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_save_file.enum_file_mode = EnumFileMode.FILE_MAY_EXIST

        self.ui.fileChooser_debug_file.enum_file_type = IodeFileType.FILE_LOG
        self.ui.fileChooser_debug_file.enum_file_mode = EnumFileMode.FILE_MAY_EXIST

        self.load_settings()

        vars_sample: Sample = variables.sample
        if vars_sample.nb_periods:
            from_period = str(vars_sample.start)
            to_period = str(vars_sample.end)
            if not self.ui.sampleEdit_sample_from.text():
                self.ui.sampleEdit_sample_from.setText(from_period)
            if not self.ui.sampleEdit_sample_to.text():
                self.ui.sampleEdit_sample_to.setText(to_period)

    @Slot()
    def import_variables(self):
        try:
            from_period: str = self.ui.sampleEdit_sample_from.text().strip()
            to_period: str = self.ui.sampleEdit_sample_to.text().strip()
            # throw an error if from_period and/or to_period are/is invalid
            Sample(from_period, to_period)

            input_file: str = self.ui.fileChooser_input_file.filepath.strip()
            rule_file: str = self.ui.fileChooser_rule_file.filepath.strip()
            save_file: str = self.ui.fileChooser_save_file.filepath.strip()
            debug_file: str = self.ui.fileChooser_debug_file.filepath.strip()

            if not input_file:
                raise ValueError("Please specify an input file.")

            if not save_file:
                raise ValueError("Please specify a save file path.")

            i_import_format = self.ui.comboBox_format.currentIndex()
            import_format: ImportFormats = self.v_import_formats[i_import_format]

            variables.convert_file(input_file, import_format, save_file, rule_file, 
                                   from_period, to_period, debug_file)

            self.accept()
        except Exception as e:
            self.parent().display_output(f"ERROR -> {str(e)}")
            QMessageBox.warning(self, "WARNING", str(e))
