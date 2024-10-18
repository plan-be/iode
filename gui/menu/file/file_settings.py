from PySide6.QtWidgets import QWidget
from PySide6.QtCore import Qt, Slot

from .ui_file_settings import Ui_MenuFileSettings
from print.file_print_preferences import FilePrintPreferences
from settings import MenuItemSettings


class MenuFileSettings(MenuItemSettings):
    def __init__(self, parent=None):
        super().__init__(parent)

        # Initialize UI components
        self.ui = Ui_MenuFileSettings()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)
        self.ui.pushButton_apply.clicked.connect(self.apply)
        self.ui.pushButton_cancel.clicked.connect(self.reject)
        self.ui.pushButton_help.clicked.connect(self.help)
        self.ui.pushButton_options.clicked.connect(self.set_print_options)

        print_destinations = ["Printer", "File"]
        self.ui.comboBox_print_dest.addItems(print_destinations)
        self.ui.comboBox_print_dest.setCurrentIndex(0)

        self.load_settings()

    def show_print_tab(self):
        """
        Switches the tab widget to the 'reports' tab.
        """
        self.ui.tabWidget.setCurrentWidget(self.ui.tab_reports)

    @Slot()
    def apply(self):
        """
        Applies the changes made in the settings.
        """
        self.accept()

    @Slot()
    def set_print_options(self):
        """
        Sets the print options.
        """
        dialog = FilePrintPreferences(self.parent())
        dialog.exec()
