from PyQt6.QtWidgets import QWidget, QMessageBox
from PyQt6.QtPrintSupport import QPrinter, QPrintPreviewDialog
from PyQt6.QtCore import pyqtSlot

from tabs.tab_text_abstract import AbstractTextWidget
from tabs.ui_tab_text import Ui_TextWidget
from utils import SHOW_IN_TEXT_TAB_EXTENSIONS_LIST

from iode import IodeFileType


class TextWidget(AbstractTextWidget):
    """
    A widget for displaying and editing text files.
    """

    def __init__(self, file_type: IodeFileType, filepath: str, parent=None):
        """
        Initializes a new instance of the TextWidget class.

        :param file_type: The type of the file to be loaded.
        :param filepath: The path of the file to be loaded.
        :param parent: The parent widget of this widget.
        """
        super().__init__(file_type, filepath, parent)

        self.ui = Ui_TextWidget()
        self._splitter = self.ui.splitter
        self._editor = self.ui.editor
        self._editor_2 = self.ui.editor_2

        # Synchronize the content of the two text editors
        self._editor_2.setDocument(self._editor.document())
        self._editor_2.hide()
        self.filter = "Text files (*" + " *".join(SHOW_IN_TEXT_TAB_EXTENSIONS_LIST) + ")"
        self._editor.modificationChanged.connect(self.setModified)

        if filepath:
            self.load(filepath, True)

    def _load(self, filepath, force_overwrite):
        """
        Loads the content of a file into the editor.

        :param filepath: The path of the file to be loaded.
        :param force_overwrite: A flag indicating whether to overwrite the current content of the editor.
        :return: True if the file was loaded successfully, False otherwise.
        """
        return self._load_(self._editor, filepath, force_overwrite)

    def save(self, filepath):
        """
        Saves the content of the editor to a file.

        :param filepath: The path of the file to be saved.
        :return: The path of the saved file.
        """
        return self._save(self._editor, filepath)

    @pyqtSlot()
    def render_for_printing(self):
        """
        Renders the content of the editor for printing.
        """
        self._editor.print(self.printer)

    @pyqtSlot()
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
