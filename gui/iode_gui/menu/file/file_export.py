from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from iode_gui.util.widgets.file_chooser import EnumFileMode
from .ui_file_export import Ui_MenuFileExport

from iode import IodeFileType, Sample, variables #, IodeExportFormat, export_as


class MenuFileExport(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuFileExport()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        # self.v_export_formats = list(IodeExportFormat)
        # v_export_formats_names = [export_format.name.title() for export_format in self.v_export_formats]
        # self.ui.comboBox_format.addItems(v_export_formats_names)
        # self.ui.comboBox_format.setCurrentIndex(0) 

        self.ui.fileChooser_comment_file.enum_file_type = IodeFileType.FILE_COMMENTS
        self.ui.fileChooser_comment_file.enum_file_mode = EnumFileMode.EXISTING_FILE

        self.ui.fileChooser_variable_file.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_variable_file.enum_file_mode = EnumFileMode.EXISTING_FILE
        
        self.ui.fileChooser_rule_file.enum_file_type = IodeFileType.FILE_ANY
        self.ui.fileChooser_rule_file.enum_file_mode = EnumFileMode.EXISTING_FILE

        self.ui.fileChooser_save_file.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_save_file.enum_file_mode = EnumFileMode.FILE_MAY_EXIST

        self.ui.fileChooser_debug_file.enum_file_type = IodeFileType.FILE_LOG
        self.ui.fileChooser_debug_file.enum_file_mode = EnumFileMode.FILE_MAY_EXIST

        self.load_settings()

        if self.ui.lineEdit_separator.text() == '':
            self.ui.lineEdit_separator.setText(';')
        if self.ui.lineEdit_nan.text() == '':
            self.ui.lineEdit_nan.setText('#N/A')

        vars_sample: Sample = variables.sample
        if vars_sample.nb_periods:
            from_period = str(vars_sample.start)
            to_period = str(vars_sample.end)
            if not self.ui.sampleEdit_sample_from.text():
                self.ui.sampleEdit_sample_from.setText(from_period)
            if not self.ui.sampleEdit_sample_to.text():
                self.ui.sampleEdit_sample_to.setText(to_period)

    @Slot()
    def save(self):
        try:
            from_period: str = self.ui.sampleEdit_sample_from.text().strip()
            to_period: str = self.ui.sampleEdit_sample_to.text().strip()
            # throw an error if from_period and/or to_period are/is invalid
            Sample(from_period, to_period)

            comment_file: str = self.ui.fileChooser_comment_file.filepath.strip()
            variable_file: str = self.ui.fileChooser_variable_file.filepath.strip()
            rule_file: str = self.ui.fileChooser_rule_file.filepath.strip()
            save_file: str = self.ui.fileChooser_save_file.filepath.strip()
            debug_file: str = self.ui.fileChooser_debug_file.filepath.strip()

            if not save_file:
                raise ValueError("Please specify a save file path.")
            
            separator: str = self.ui.lineEdit_separator.text().strip()[0]
            if not separator:
                raise ValueError("Please specify a separator.")

            nan_value = self.ui.lineEdit_nan.text().strip()
            if not nan_value:
                raise ValueError("Please specify a value for missing data (nan).")

            # i_export_format = self.ui.comboBox_format.currentIndex()
            # export_format: IodeExportFormat = self.v_export_formats[i_export_format]

            # export_as(variable_file, comment_file, from_period, to_period, export_format, 
            #           save_file, rule_file, nan_value, separator, debug_file)

            raise NotImplementedError("This feature is not yet implemented.")

            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", str(e))
