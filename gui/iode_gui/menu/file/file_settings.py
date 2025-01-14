from PySide6.QtCore import Qt, Slot, QSettings
from PySide6.QtWidgets import QWidget

from iode_gui.print.file_print_preferences import FilePrintPreferences
from iode_gui.settings import (ProjectSettings, MixinSettingsDialog, 
                               PRINT_DESTINATION, RUN_REPORTS_FROM_PROJECT_DIR)
from .ui_file_settings import Ui_MenuFileSettings


class MenuFileSettings(MixinSettingsDialog):
    def __init__(self, parent=None):
        super().__init__(parent)

        # Initialize UI components
        self.ui = Ui_MenuFileSettings()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        print_destinations = ["Printer", "File"]
        self.ui.comboBox_print_dest.addItems(print_destinations)
        self.ui.comboBox_print_dest.setCurrentIndex(0)

        self.load_settings()

    def show_print_tab(self):
        """
        Switches the tab widget to the 'print' tab.
        """
        self.ui.tabWidget.setCurrentWidget(self.ui.tab_print)

    def show_reports_tab(self):
        """
        Switches the tab widget to the 'reports' tab.
        """
        self.ui.tabWidget.setCurrentWidget(self.ui.tab_reports)

    @Slot()
    def apply(self):
        """
        Applies the changes made in the settings.
        """
        project_settings: QSettings = ProjectSettings.project_settings
        if project_settings is not None:
            print_to_file: bool = self.ui.comboBox_print_dest.currentText() == "File"
            project_settings.setValue(PRINT_DESTINATION, print_to_file)
            project_settings.setValue(RUN_REPORTS_FROM_PROJECT_DIR, 
                                      self.ui.radioButton_run_from_project_dir.isChecked())
        self.accept()

    @Slot()
    def set_print_options(self):
        """
        Sets the print options.
        """
        dialog = FilePrintPreferences(self.parent())
        dialog.exec()
