from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from settings import MixinSettingsDialog
from util.widgets.file_chooser import EnumFileMode
from .ui_workspace_high_to_low import Ui_MenuWorkspaceHighToLow

from iode import IodeFileType, variables, HighToLowType


class MenuWorkspaceHighToLow(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuWorkspaceHighToLow()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.ui.fileChooser_input_file.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_input_file.enum_file_mode = EnumFileMode.EXISTING_FILE

    @Slot()
    def slot_high_to_low(self):
        try:
            filepath: str = self.ui.fileChooser_input_file.filepath
            last_obs_series: str = ";".join(self.ui.textEdit_last_observation.toPlainText().splitlines()).strip()
            mean_series: str = ";".join(self.ui.textEdit_mean.toPlainText().splitlines()).strip()
            sum_series: str = ";".join(self.ui.textEdit_sum.toPlainText().splitlines()).strip()

            if not len(filepath):
                raise ValueError("'input file' is empty")

            # Check if all series names are empty
            if not last_obs_series and not mean_series and not sum_series:
                raise ValueError("The series 'last observation', 'mean' and 'sum' are all empty")

            # Call high_to_low function with appropriate arguments
            if last_obs_series:
                variables.high_to_low(HighToLowType.LAST, filepath, last_obs_series)
            if mean_series:
                variables.high_to_low(HighToLowType.MEAN, filepath, mean_series)
            if sum_series:
                variables.high_to_low(HighToLowType.SUM, filepath, sum_series)

            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "Failed to build series of lower periodicity.\n" + str(e))
