import sys
from typing import List

from PySide6.QtCore import Qt, QSettings
from PySide6.QtGui import QColor
from PySide6.QtWidgets import QMessageBox
from PySide6 import __version__ as pyside_version
v_major, v_minor, v_patch = map(int, pyside_version.split('.'))
pyside6_68_or_higher = (v_major, v_minor) >= (6, 8)


class ColorTheme:
    def __init__(self, mode: str, header_background: QColor, header_text_color: QColor, report_internal_functions: QColor, 
                 report_commands: QColor, report_functions: QColor, report_comments: QColor, report_macros: QColor, report_expressions: QColor):
        self.mode: str = mode
        # background color for headers in QTableView
        self.header_background: QColor = header_background
        # text color for headers in QTableView
        self.header_text_color: QColor = header_text_color
        # report command -> $ or # (group 0 -> Report internal fns)
        self.report_internal_functions: QColor = report_internal_functions
        # report command -> $ or # (group 1)
        self.report_commands: QColor = report_commands
        # report function -> @
        self.report_functions: QColor = report_functions
        # report comment
        self.report_comments: QColor = report_comments
        # %<expression>%
        self.report_macros: QColor = report_macros
        # {expression}
        self.report_expressions: QColor = report_expressions

    def update_settings(self, settings: QSettings) -> None:
        """Update the settings with the current color theme"""
        if settings is None:
            return
        if pyside6_68_or_higher:
            settings.setValue("color_theme", self.mode)
        settings.beginGroup(f"{self.mode}_mode")
        settings.setValue("header_background", self.header_background.name())
        settings.setValue("header_text_color", self.header_text_color.name())
        settings.setValue("report_internal_functions", self.report_internal_functions.name())
        settings.setValue("report_commands", self.report_commands.name())
        settings.setValue("report_functions", self.report_functions.name())
        settings.setValue("report_comments", self.report_comments.name())
        settings.setValue("report_macros", self.report_macros.name())
        settings.setValue("report_expressions", self.report_expressions.name())
        settings.endGroup()

    @classmethod
    def from_settings(cls, settings: QSettings, mode: str=None) -> 'ColorTheme':
        """Create a ColorTheme instance from QSettings"""
        if settings is None:
            return default_dark_mode if is_dark_mode() else default_light_mode
        
        if mode is None:
            if pyside6_68_or_higher: 
                if not settings.contains("color_theme"):
                    return default_dark_mode if is_dark_mode() else default_light_mode
                mode = settings.value("color_theme")
            else:
                mode = 'dark' if is_dark_mode() else 'light'

        group_name = f"{mode}_mode"
        
        if mode == 'light' and group_name not in settings.childGroups():
            return default_light_mode
        
        if mode == 'dark' and group_name not in settings.childGroups():
            return default_dark_mode
        
        try:
            settings.beginGroup(group_name)
            header_background = QColor(settings.value("header_background"))
            header_text_color = QColor(settings.value("header_text_color"))
            report_internal_functions = QColor(settings.value("report_internal_functions"))
            report_commands = QColor(settings.value("report_commands"))
            report_functions = QColor(settings.value("report_functions"))
            report_comments = QColor(settings.value("report_comments"))
            report_macros = QColor(settings.value("report_macros"))
            report_expressions = QColor(settings.value("report_expressions"))
            settings.endGroup()
            
            return cls(mode, header_background, header_text_color, report_internal_functions, 
                    report_commands, report_functions, report_comments, report_macros, report_expressions)
        except Exception as e:
            QMessageBox.warning(None, "Error", f"Failed to load color theme from settings: {e}")
            return default_dark_mode if is_dark_mode() else default_light_mode


default_light_mode = ColorTheme(
    mode='light',
    header_background = QColor.fromString("#c0c0c0"),
    header_text_color = QColor(Qt.GlobalColor.black),
    report_internal_functions = QColor(Qt.GlobalColor.red),
    report_commands = QColor(Qt.GlobalColor.darkBlue),
    report_functions = QColor(Qt.GlobalColor.darkGreen),
    report_comments = QColor.fromString("#808000"),
    report_macros = QColor(Qt.GlobalColor.darkMagenta),
    report_expressions = QColor(Qt.GlobalColor.darkGreen))


default_dark_mode = ColorTheme(
    mode='dark',
    header_background = QColor.fromString("#a0a0a4"),
    header_text_color = QColor.fromString("#f0f0f0"),
    report_internal_functions = QColor.fromString("#FA6675"),
    report_commands = QColor.fromString("#57AAF7"),
    report_functions = QColor.fromString("#67A37C"),
    report_comments = QColor.fromString("#5F826B"),
    report_macros = QColor.fromString("#C77DBB"),
    report_expressions = QColor.fromString("#67A37C"))


color_themes: List[str] = ['light', 'dark']


def is_dark_mode():
    if sys.platform != "win32":
        return False
    try:
        import winreg
        registry = winreg.ConnectRegistry(None, winreg.HKEY_CURRENT_USER)
        key = winreg.OpenKey(registry, r"Software\Microsoft\Windows\CurrentVersion\Themes\Personalize")
        value, _ = winreg.QueryValueEx(key, "AppsUseLightTheme")
        return value == 0  # 0 means dark mode, 1 means light mode
    except Exception:
        return False