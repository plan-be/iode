from PySide6.QtWidgets import QWidget, QMessageBox, QSplitter
from PySide6.QtPrintSupport import QPrinter, QPrintPreviewDialog
from PySide6.QtCore import Slot

from iode_gui.abstract_main_window import AbstractMainWindow
from iode_gui.tabs.tab_text_abstract import AbstractTextWidget
from iode_gui.text_edit.text_editor import IodeTextEditor
from iode_gui.tabs.ui_tab_text import Ui_TextWidget
from iode_gui.utils import SHOW_IN_TEXT_TAB_EXTENSIONS_LIST

from iode import IodeFileType


class TextWidget(AbstractTextWidget):
    """
    A widget for displaying and editing text files.
    """

    def __init__(self, file_type: IodeFileType, filepath: str, main_window: AbstractMainWindow, parent=None):
        """
        Initializes a new instance of the TextWidget class.

        :param file_type: The type of the file to be loaded.
        :param filepath: The path of the file to be loaded.
        :param parent: The parent widget of this widget.
        """
        super().__init__(file_type, filepath, main_window, parent)

        self.ui = Ui_TextWidget()
        self.ui.setupUi(self)
        self.splitter: QSplitter = self.ui.splitter
        self._editor: IodeTextEditor = self.ui.editor
        self._editor_2: IodeTextEditor = self.ui.editor_2

        # Synchronize the content of the two text editors
        self._editor_2.setDocument(self._editor.document())
        self._editor_2.hide()

        self.filter = "Text files (*" + " *".join(SHOW_IN_TEXT_TAB_EXTENSIONS_LIST) + ")"
        self._editor.modificationChanged.connect(self.set_modified)

        if filepath:
            self.load(filepath, True)

    def _load(self, filepath, force_overwrite) -> bool:
        """
        Loads the content of a file into the editor.

        :param filepath: The path of the file to be loaded.
        :param force_overwrite: not used here, but needed for the abstract class.
        :return: True if the file was loaded successfully, False otherwise.
        """
        return self._load_(filepath)

    @Slot()
    def render_for_printing(self):
        """
        Renders the content of the editor for printing.
        """
        self._editor.print_(self.printer)

    @Slot()
    def print(self):
        """
        Opens a print preview dialog for printing the content of the editor.
        """
        try:
            dialog = QPrintPreviewDialog(self.printer)
            dialog.paintRequested.connect(self.render_for_printing)
            dialog.exec()
        except Exception as e:
            QMessageBox.warning(None, "Print Graph", f"Could not print graph\n{str(e)}")
