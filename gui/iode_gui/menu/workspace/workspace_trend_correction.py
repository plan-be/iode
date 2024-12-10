from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from iode_gui.util.widgets.file_chooser import EnumFileMode
from .ui_workspace_trend_correction import Ui_MenuWorkspaceTrendCorrection

from iode import IodeFileType, variables


class MenuWorkspaceTrendCorrection(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuWorkspaceTrendCorrection()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.ui.fileChooser_input_file.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_input_file.enum_file_mode = EnumFileMode.EXISTING_FILE

        self.load_settings()

    @Slot()
    def trend_correction(self):
        try:
            filepath: str = self.ui.fileChooser_input_file.filepath
            series: str = ";".join(self.ui.textEdit_series.toPlainText().splitlines()).strip()
            lambda_value: float = self.ui.doubleSpinBox_lambda.value()
            log: bool = self.ui.checkBox_log.isChecked()

            if not len(filepath):
                raise ValueError("'input file' is empty")

            variables.trend_correction(filepath, lambda_value, series, log)
            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "Failed to compute trend correction.\n" + str(e))
