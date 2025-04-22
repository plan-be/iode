from PySide6.QtCore import Qt, Slot, QSettings
from PySide6.QtWidgets import QWidget

from iode_gui.print.file_print_preferences import FilePrintPreferences
from iode_gui.settings import (get_settings, MixinSettingsDialog, 
                               PRINT_TO_FILE, RUN_REPORTS_FROM_PROJECT_DIR)
from iode_gui.abstract_main_window import AbstractMainWindow
from .ui_file_settings import Ui_MenuFileSettings


class MenuFileSettings(MixinSettingsDialog):
    def __init__(self, parent: AbstractMainWindow):
        super().__init__(parent)

        # Initialize UI components
        self.ui = Ui_MenuFileSettings()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        PRINT_TO_FILEs = ["Printer", "File"]
        self.ui.comboBox_print_dest.addItems(PRINT_TO_FILEs)
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
        settings: QSettings = get_settings()
        if settings is not None:
            print_to_file: bool = self.ui.comboBox_print_dest.currentText() == "File"
            settings.setValue(PRINT_TO_FILE, print_to_file)
            settings.setValue(RUN_REPORTS_FROM_PROJECT_DIR, 
                              self.ui.radioButton_run_from_project_dir.isChecked())
        self.accept()

    @Slot()
    def set_print_options(self):
        """
        Sets the print options.
        """
        dialog = FilePrintPreferences(self.parent())
        dialog.exec()
