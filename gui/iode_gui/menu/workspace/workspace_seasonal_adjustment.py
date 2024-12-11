from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from util.widgets.file_chooser import EnumFileMode
from .ui_workspace_seasonal_adjustment import Ui_MenuWorkspaceSeasonalAdjustment

from iode import IodeFileType, variables


class MenuWorkspaceSeasonalAdjustment(MixinSettingsDialog):
    def __init__(self, parent: QWidget=None):
        super().__init__(parent)
        self.ui = Ui_MenuWorkspaceSeasonalAdjustment()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.ui.fileChooser_input_file.enum_file_type = IodeFileType.FILE_VARIABLES
        self.ui.fileChooser_input_file.enum_file_mode = EnumFileMode.EXISTING_FILE

        self.load_settings()

    @Slot()
    def seasonal_adjustment(self):
        try:
            filepath: str = self.ui.fileChooser_input_file.filepath
            series: str = ";".join(self.ui.textEdit_series.toPlainText().splitlines()).strip()
            EPS_test: float = self.ui.doubleSpinBox_EPS_test.value()

            if not len(filepath):
                raise ValueError("'input file' is empty")

            variables.seasonal_adjustment(filepath, EPS_test, series)
            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", "Failed to compute seasonal adjustment.\n" + str(e))
