from PySide6.QtCore import Qt, QDir, QFileInfo
from PySide6.QtWidgets import QTextEdit, QMessageBox

from iode_gui.settings import ProjectSettings, RUN_REPORTS_FROM_PROJECT_DIR
from .highlighter import IodeHighlighter
from .completer import IodeCompleter
from .text_editor import IodeTextEditor
from iode_gui.abstract_main_window import AbstractMainWindow

from iode import TableLang, execute_report, execute_command
from typing import Union, List


class IodeReportEditor(IodeTextEditor):
    """
    Widget allowing to edit an Iode report.

    CTRL+Space: forces auto-completion
    Tab | Return | Space: stops auto-completion

    For syntax highlight, see example https://doc.qt.io/qt-6/qtwidgets-richtext-syntaxhighlighter-example.html
    For auto-completion, see example https://doc.qt.io/qt-6/qtwidgets-tools-customcompleter-example.html
    """

    def __init__(self, parent=None):
        super().__init__(parent)
        self.highlighter: IodeHighlighter = IodeHighlighter(self.document())
        self.output: QTextEdit = None

    def setup(self, main_window: AbstractMainWindow):
        """
        Setup the IodeReportEditor.
        """
        self.output: QTextEdit = main_window.output

    def run(self, filepath: str, parameters: Union[str, List[str]], nb_decimals: int, language: TableLang):
        """
        Executes an IODE report.

        Note: See function C_ReportExec() from the file sb_rep.c from the old GUI.

        Args:
            filepath (str): The path of the report file.
            parameters (str): The parameters for the report.
            nb_decimals (int): The number of decimals to use.
            language (TableLang): The language to use.
        """
        current_project_dir = QDir.currentPath()
        project_settings = ProjectSettings.project_settings
        if project_settings:
            run_from_project_dir = project_settings.value(RUN_REPORTS_FROM_PROJECT_DIR, 
                                                          defaultValue=True, type=bool)
        else:
            run_from_project_dir = True

        if not isinstance(parameters, str) and not all(isinstance(p, str) for p in parameters):
            raise ValueError("'parameters' must be a string or a list of strings")
        
        if not isinstance(parameters, str):
            parameters = ";".join(parameters)

        try:
            if self.output:
                self.output.setTextColor(Qt.GlobalColor.darkBlue)
                self.output.append(f"\nRunning report {filepath} ...\n")
                self.output.setTextColor(Qt.GlobalColor.black)

            # set IODE global variables nb_decimals and language
            execute_command(f"$PrintNbDec  {nb_decimals}")
            execute_command(f"$PrintLang  {language.name.title()}")

            # updates current directory execution (chdir)
            if not run_from_project_dir:
                QDir.setCurrent(QFileInfo(filepath).absolutePath())

            # executes IODE report
            execute_report(filepath, parameters)

            # reset current directory execution to project directory (chdir)
            if not run_from_project_dir:
                QDir.setCurrent(current_project_dir)

            success = True
        except Exception as e:
            # reset current directory execution to project directory (chdir)
            if not run_from_project_dir:
                QDir.setCurrent(current_project_dir)

            success = False
            msg = str(e)
            QMessageBox.warning(None, "WARNING", f"{msg}\n")

        if self.output:
            if success:
                self.output.setTextColor(Qt.GlobalColor.darkGreen)
                msg = "Successful"
            else:
                self.output.setTextColor(Qt.GlobalColor.red)
                msg = "Failed"

            self.output.append(f"\nExecution of report {filepath}: {msg}\n")
            self.output.setTextColor(Qt.GlobalColor.black)
