from PySide6.QtCore import Signal, Slot
from PySide6.QtWidgets import QWidget, QSplitter, QMessageBox
from PySide6.QtPrintSupport import QPrintPreviewDialog

from iode import IodeFileType, TableLang

from iode_gui.utils import IODE_REPORT_EXTENSION
from iode_gui.settings import ProjectSettings
from iode_gui.abstract_main_window import AbstractMainWindow
from iode_gui.text_edit.report_editor import IodeReportEditor
from iode_gui.tabs.tab_text_abstract import AbstractTextWidget
from iode_gui.tabs.ui_tab_report import Ui_ReportWidget


class ReportWidget(AbstractTextWidget):
    """
    Allows to edit and run an Iode report.

    Implemented features:
    - Syntax color highlight.
    - Auto-completion on $/#/@-functions + all Iode objects names.
    - Shortcuts:
      -> CTRL + R runs the report.
    """

    ask_compute_hash = Signal(bool)

    def __init__(self, filepath: str, main_window: AbstractMainWindow, parent: QWidget=None):
        """
        Constructor for ReportWidget.

        :param filepath: The path to the report file.
        :param parent: The parent widget.
        """
        super().__init__(IodeFileType.FILE_REP, filepath, main_window, parent)
        
        self.ui = Ui_ReportWidget()
        self.ui.setupUi(self)
        self.splitter: QSplitter = self.ui.splitter
        self._editor: IodeReportEditor = self.ui.editor
        self._editor_2: IodeReportEditor = self.ui.editor_2

        # Synchronizes the content of the two Report editors
        self._editor_2.setDocument(self._editor.document())

        # Setup the editors
        self._editor.setup(main_window)
        self._editor_2.setup(main_window)

        self._editor_2.hide()

        self.v_lang_names = [name.upper() for name in TableLang.__members__]
        self.v_langs = list(TableLang)
        self.ui.comboBox_language.addItems(self.v_lang_names)
        self.ui.comboBox_language.setEditable(False)

        self.filter = f"IODE report files (*{IODE_REPORT_EXTENSION})"

        # Connect signals to slots
        self._editor.modificationChanged.connect(self.set_modified)

        if filepath:
            self.load(filepath, True)

    def _load(self, filepath, force_overwrite) -> bool:
        """
        Load the report from the given filepath.

        :param filepath: The path to the report file.
        :param force_overwrite: not used here, but needed for the abstract class.
        :return: True if the report was loaded successfully, False otherwise.
        """
        return self._load_(filepath)

    def save(self, filepath):
        """
        Save the report to the given filepath.

        :param filepath: The path to the report file.
        """
        return self._save(filepath)

    def load_settings(self):
        """
        Load the settings for the report from the given project settings.
        """
        project_settings = ProjectSettings.project_settings
        if not project_settings:
            return

        parameters = project_settings.value("report_parameters", "")
        language_index = project_settings.value("report_language", 0, type=int)
        nb_decimals = project_settings.value("report_nb_decimals", 2, type=int)

        self.ui.lineEdit_parameters.setText(parameters)
        self.ui.comboBox_language.setCurrentIndex(language_index)
        self.ui.spinBox_nbDecimals.setValue(nb_decimals)

    def save_settings(self):
        """
        Save the settings for the report to the given project settings.
        """
        project_settings = ProjectSettings.project_settings
        if not project_settings:
            return

        parameters = self.ui.lineEdit_parameters.text()
        language_index = self.ui.comboBox_language.currentIndex()
        nb_decimals = self.ui.spinBox_nbDecimals.value()

        project_settings.setValue("report_parameters", parameters)
        project_settings.setValue("report_language", language_index)
        project_settings.setValue("report_nb_decimals", nb_decimals)

    @Slot()
    def run(self):
        """
        Run the report.
        """
        self.ask_compute_hash.emit(True)

        # Save the current content of the report tab because the user may have
        # made some modifications in the report editor which are not saved yet.
        self.save(self.filepath)

        parameters = self.ui.lineEdit_parameters.text()
        i_language = self.ui.comboBox_language.currentIndex()
        language = self.v_langs[i_language]
        nb_decimals = self.ui.spinBox_nbDecimals.value()
        self._editor.run(self.filepath, parameters, nb_decimals, language)
        self.main_window.update_tab_and_completer()

    @Slot()
    def _render_for_printing(self):
        """
        Render the report for printing.
        """
        self._editor.print_(self.printer)

    @Slot()
    def print(self):
        """
        Print the report.
        """
        try:
            dialog = QPrintPreviewDialog(self.printer)
            dialog.paintRequested.connect(self._render_for_printing)
            dialog.exec()
        except Exception as e:
            QMessageBox.warning(None, "Print Graph", f"Could not print graph\n{str(e)}")
