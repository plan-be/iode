from qtpy.QtCore import Qt, Slot, QSettings
from qtpy.QtWidgets import QWidget, QApplication, QColorDialog, QMessageBox
from qtpy.QtGui import QStyleHints, QColor
from qtpy import __version__ as pyside_version
v_major, v_minor, v_patch = map(int, pyside_version.split('.'))
pyside6_68_or_higher = (v_major, v_minor) >= (6, 8)

from iode_gui.print.file_print_preferences import FilePrintPreferences
from iode_gui.settings import (get_settings, get_color_theme, set_color_theme,
                               MixinSettingsDialog, PRINT_TO_FILE, RUN_REPORTS_FROM_PROJECT_DIR)
from iode_gui.color_theme import (ColorTheme, color_themes, is_dark_mode, 
                                  default_light_mode, default_dark_mode)
from iode_gui.abstract_main_window import AbstractMainWindow
from iode_gui.text_edit.highlighter import IodeHighlighter
from .ui_file_settings import Ui_MenuFileSettings


class MenuFileSettings(MixinSettingsDialog):
    def __init__(self, parent: AbstractMainWindow):
        super().__init__(parent)

        app = QApplication.instance()
        self.style_hints: QStyleHints = app.styleHints()
        self.color_scheme = self.style_hints.colorScheme

        self.color_theme: ColorTheme = get_color_theme()
        self.mode: str = self.color_theme.mode

        # Initialize UI components
        self.ui = Ui_MenuFileSettings()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        document = self.ui.plainTextEdit_report_example.document()
        self.highlighter: IodeHighlighter = IodeHighlighter(document)

        PRINT_TO_FILES = ["Printer", "File"]
        self.ui.comboBox_print_dest.addItems(PRINT_TO_FILES)
        self.ui.comboBox_print_dest.setCurrentIndex(0)

        self.skip_update_colors: bool = True

        if pyside6_68_or_higher:
            self.ui.comboBox_color_theme.addItems(color_themes)
        else:
            self.ui.label_color_theme.hide()
            self.ui.comboBox_color_theme.hide()
            self.ui.pushButton_detect_color_theme.hide()

        self.ui.pushButton_header_background.clicked.connect(self.set_color_from_qdialog_color)
        self.ui.pushButton_header_text.clicked.connect(self.set_color_from_qdialog_color)
        self.ui.pushButton_report_internal_functions.clicked.connect(self.set_color_from_qdialog_color)
        self.ui.pushButton_report_commands.clicked.connect(self.set_color_from_qdialog_color)
        self.ui.pushButton_report_functions.clicked.connect(self.set_color_from_qdialog_color)
        self.ui.pushButton_report_comments.clicked.connect(self.set_color_from_qdialog_color)
        self.ui.pushButton_report_macros.clicked.connect(self.set_color_from_qdialog_color)
        self.ui.pushButton_report_expressions.clicked.connect(self.set_color_from_qdialog_color)

        self.ui.lineEdit_header_background.textChanged.connect(self.set_color_from_lineedit)
        self.ui.lineEdit_header_text.textChanged.connect(self.set_color_from_lineedit)
        self.ui.lineEdit_report_internal_functions.textChanged.connect(self.set_color_from_lineedit)
        self.ui.lineEdit_report_commands.textChanged.connect(self.set_color_from_lineedit)
        self.ui.lineEdit_report_functions.textChanged.connect(self.set_color_from_lineedit)
        self.ui.lineEdit_report_comments.textChanged.connect(self.set_color_from_lineedit)
        self.ui.lineEdit_report_macros.textChanged.connect(self.set_color_from_lineedit)
        self.ui.lineEdit_report_expressions.textChanged.connect(self.set_color_from_lineedit)

        self.rejected.connect(self.reset_color_scheme)

        self.load_settings()

        if pyside6_68_or_higher:
            self.ui.comboBox_color_theme.setCurrentText(self.mode)

        self.update_qt_color_sheme(self.mode)
        
        header_background = self.color_theme.header_background.name()
        header_text = self.color_theme.header_text_color.name()
        report_internal_functions = self.color_theme.report_internal_functions.name()
        report_commands = self.color_theme.report_commands.name()
        report_functions = self.color_theme.report_functions.name()
        report_comments = self.color_theme.report_comments.name()
        report_macros = self.color_theme.report_macros.name()
        report_expressions = self.color_theme.report_expressions.name()
        
        self.ui.lineEdit_header_background.setText(header_background)
        self.ui.lineEdit_header_text.setText(header_text)
        self.ui.lineEdit_report_internal_functions.setText(report_internal_functions)
        self.ui.lineEdit_report_commands.setText(report_commands)
        self.ui.lineEdit_report_functions.setText(report_functions)
        self.ui.lineEdit_report_comments.setText(report_comments)
        self.ui.lineEdit_report_macros.setText(report_macros)
        self.ui.lineEdit_report_expressions.setText(report_expressions)

        text = "# ----------------------------------------------------------\n"
        text += "$ sumvars: creates a new series names %result% = sum of %vars%\n"
        text += "$procdef sumvars result vars\n"
        text += "    $define eq 0\n"
        text += "    $foreach var %vars%\n"
        text += "        $define eq %eq% + %var%\n"
        text += "    $next var\n"
        text += "    $DataCalcVar %result% %eq%\n"
        text += "$procend\n"
        text += "# ----------------------------------------------------------\n"
        text += "\n"
        text += "$WsSample 1990Y1 2000Y1\n"
        text += "$DataCalcVar PIB_A t\n"
        text += "$DataCalcVar PIB_B ln t\n"
        text += "$DataCalcVar PIB_C sin(t)\n"
        text += "$procexec sumvarsmsg \"Sum PIB:\" PIB PIB_A PIB_B PIB_C\n"
        self.ui.plainTextEdit_report_example.setPlainText(text)

        self.highlighter.set_color_theme(self.color_theme)

        self.skip_update_colors = False

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

    def update_qt_color_sheme(self, mode: str):
        if pyside6_68_or_higher:
            color_scheme = Qt.ColorScheme.Dark if mode == 'dark' else Qt.ColorScheme.Light
            self.style_hints.setColorScheme(color_scheme)

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
            if self.color_theme is not None:
                set_color_theme(self.color_theme)
                self.color_theme.update_settings(settings)

        if self.ui.checkBox_default_color_theme.isChecked() and self.color_theme is not None:
            user_settings: QSettings = QSettings(QSettings.Scope.UserScope)
            self.color_theme.update_settings(user_settings)

        self.accept()

    @Slot()
    def set_print_options(self):
        """
        Sets the print options.
        """
        dialog = FilePrintPreferences(self.parent())
        dialog.exec()

    @Slot()
    def detect_color_theme(self):
        """
        Detects the current color theme and updates the UI accordingly.
        """
        if is_dark_mode():
            self.load_colors('dark')
        else:
            self.load_colors('light')

    @Slot()
    def reset_color_scheme(self):
        """
        Resets the color scheme to the one set before opening the menu.
        """
        self.update_qt_color_sheme(self.mode)

    @Slot()
    def reset_colors(self):
        self.skip_update_colors = True

        mode = self.ui.comboBox_color_theme.currentText() if pyside6_68_or_higher else self.mode        
        color_theme = default_dark_mode if mode == 'dark' else default_light_mode

        self.ui.lineEdit_header_background.setText(color_theme.header_background.name())
        self.ui.lineEdit_header_text.setText(color_theme.header_text_color.name())
        self.ui.lineEdit_report_internal_functions.setText(color_theme.report_internal_functions.name())
        self.ui.lineEdit_report_commands.setText(color_theme.report_commands.name())
        self.ui.lineEdit_report_functions.setText(color_theme.report_functions.name())
        self.ui.lineEdit_report_comments.setText(color_theme.report_comments.name())
        self.ui.lineEdit_report_macros.setText(color_theme.report_macros.name())
        self.ui.lineEdit_report_expressions.setText(color_theme.report_expressions.name())

        self.skip_update_colors = False
        self.update_colors(mode)

    @Slot()
    def set_color_from_qdialog_color(self):
        button = self.sender()
        if button is None:
            return

        color = QColor(button.palette().button().color())
        color = QColorDialog.getColor(color, self, "Select Color")
        if not color.isValid():
            return

        button.setStyleSheet(f"background-color: {color.name()};")

        item_name = button.objectName().replace("pushButton_", "")
        line_edit = getattr(self.ui, f"lineEdit_{item_name}", None)
        if line_edit:
            line_edit.setText(color.name())

        mode = self.ui.comboBox_color_theme.currentText() if pyside6_68_or_higher else self.mode
        self.update_colors(mode)

    @Slot(str)
    def set_color_from_lineedit(self, color_name: str):
        line_edit = self.sender()
        if line_edit is None:
            return

        if not color_name:
            return

        color = QColor(color_name)
        if not color.isValid():
            return

        item_name = line_edit.objectName().replace("lineEdit_", "")
        button = getattr(self.ui, f"pushButton_{item_name}", None)
        if button:
            button.setStyleSheet(f"background-color: {color.name()};")

        mode = self.ui.comboBox_color_theme.currentText() if pyside6_68_or_higher else self.mode
        self.update_colors(mode)

    @Slot(str)
    def load_colors(self, mode: str):
        """
        Loads the colors from the current color theme into the UI.
        """
        if not pyside6_68_or_higher:
            return

        self.skip_update_colors = True

        try:
            self.ui.comboBox_color_theme.setCurrentText(mode)
            self.color_theme = ColorTheme.from_settings(get_settings(), mode)
            
            self.ui.lineEdit_header_background.setText(self.color_theme.header_background.name())
            self.ui.lineEdit_header_text.setText(self.color_theme.header_text_color.name())
            self.ui.lineEdit_report_internal_functions.setText(self.color_theme.report_internal_functions.name())
            self.ui.lineEdit_report_commands.setText(self.color_theme.report_commands.name())
            self.ui.lineEdit_report_functions.setText(self.color_theme.report_functions.name())
            self.ui.lineEdit_report_comments.setText(self.color_theme.report_comments.name())
            self.ui.lineEdit_report_macros.setText(self.color_theme.report_macros.name())
            self.ui.lineEdit_report_expressions.setText(self.color_theme.report_expressions.name())

            self.update_qt_color_sheme(mode)
            self.highlighter.set_color_theme(self.color_theme)
        except Exception as e:
            QMessageBox.warning(f"Error loading colors for the '{mode}' mode: {e}")
        
        self.skip_update_colors = False

    @Slot(str)
    def update_colors(self, mode: str):
        """
        Sets the color theme based on the selected option.
        """
        if not self.skip_update_colors and mode in color_themes:
            try:
                header_background = self.ui.lineEdit_header_background.text()
                header_text = self.ui.lineEdit_header_text.text()
                report_internal_functions = self.ui.lineEdit_report_internal_functions.text()
                report_commands = self.ui.lineEdit_report_commands.text()
                report_functions = self.ui.lineEdit_report_functions.text()            
                report_comments = self.ui.lineEdit_report_comments.text()
                report_macros = self.ui.lineEdit_report_macros.text()
                report_expressions = self.ui.lineEdit_report_expressions.text()  

                self.color_theme = ColorTheme(
                    mode = mode,
                    header_background=QColor(header_background),
                    header_text_color=QColor(header_text),
                    report_internal_functions=QColor(report_internal_functions),
                    report_commands=QColor(report_commands),
                    report_functions=QColor(report_functions),
                    report_comments=QColor(report_comments),
                    report_macros=QColor(report_macros),
                    report_expressions=QColor(report_expressions)
                )

                self.update_qt_color_sheme(mode)
                self.highlighter.set_color_theme(self.color_theme)
            except Exception as e:
                QMessageBox.warning(self, "Error", f"Failed to update colors for the '{mode}' mode: {e}")
