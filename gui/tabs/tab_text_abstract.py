from PySide6.QtWidgets import QWidget, QPlainTextEdit, QMessageBox, QGridLayout, QFileDialog
from PySide6.QtCore import Signal, Slot, Qt, QFileInfo, QTextStream
from PySide6.QtPrintSupport import QPrinter, QPrintPreviewDialog

from abstract_main_window import AbstractMainWindow
from tabs.tab_abstract import IodeAbstractWidget

from typing import Optional
from iode import IodeFileType


class AbstractTextWidget(IodeAbstractWidget):
    """
    This class represents an abstract text widget.
    """

    tab_text_modified = Signal(str, bool)

    def __init__(self, file_type: IodeFileType, filepath: str, main_window: AbstractMainWindow, parent=None):
        """
        Initializes the AbstractTextWidget.

        :param file_type: The type of the file.
        :param filepath: The path of the file.
        :param parent: The parent widget.
        """
        super().__init__(file_type, parent)
        self.main_window = main_window
        self._filepath = filepath
        self.filter = ""
        self.printer = QPrinter()
        self._editor: QPlainTextEdit = None
        self._editor_2: QPlainTextEdit = None

    @property
    def filepath(self) -> str:
        return self._filepath

    @filepath.setter
    def filepath(self, filepath: str):
        if self._check_new_filepath(filepath):
            self._filepath = filepath
            self.set_modified(False)

    def update(self):
        """
        Updates the widget.
        """
        pass

    def _load(filepath: str, force_overwrite: bool) -> bool:
        raise NotImplementedError()

    def _load_(self, filepath: str) -> bool:
        """
        Loads the file into the editor.

        :param filepath: The path of the file to load.
        :return: True if the file was loaded, False otherwise.
        """
        # The fileChanged signal from the QFileSystemWatcher member of the IodeAbstractTabWidget class
        # is sent when the content of a tab is saved to file. The reloadFile() method of the same class
        # resets the cursor to the beginning and resets the stack of undo and redo.
        # We don't want that when saving the content of the tab to the corresponding file.
        if self.saving_file:
            self.saving_file = False
            return False

        try:
            with open(filepath, 'r') as file:
                file_content = file.read()
                if self._editor.toPlainText() == file_content:
                    return False

                self._editor.setPlainText(file_content)
                # updates modified status -> automatically calls setModified
                self._editor.document().setModified(False)
                return True
        except Exception as e:
            QMessageBox.warning(None, "WARNING", f"Could not load file {filepath}\n\n{str(e)}")
            return False

    def _save(self, filepath: str) -> Optional[str]:
        """
        Saves the content of the editor to the file.

        :param editor: The editor to save the content from.
        :param filepath: The path of the file to save to.
        :return: The path of the file.
        """
        if not filepath:
            return filepath

        # The fileChanged signal from the QFileSystemWatcher member of the IodeAbstractTabWidget class
        # is sent when the content of a tab is saved to file. The reloadFile() method of the same class
        # resets the cursor to the beginning and resets the stack of undo and redo.
        self.saving_file = True

        try:
            with open(filepath, 'w') as file:
                file.write(self._editor.toPlainText() + "\n")
                # updates modified status -> automatically calls setModified
                self._editor.document().setModified(False)
                return filepath
        except Exception as e:
            self.savingFile = False
            QMessageBox.warning(None, "WARNING", f"Could not save tab content to file {filepath}\n\n{str(e)}")
            return None

    def save(self, filepath: str) -> str:
        """
        Saves the content to the file.

        :param filepath: The path of the file to save to.
        :return: The path of the file.
        """
        raise NotImplementedError()

    def save(self) -> Optional[str]:
        """
        Saves the content to the current filepath.

        :return: The path of the file.
        """
        return self.save(self.filepath)

    def _save_as(self) -> Optional[str]:
        """
        Saves the content to a new file.

        :return: The path of the new file.
        """
        new_filepath, _ = QFileDialog.getSaveFileName(self, "Save As", None, self.filter)
        if not new_filepath:
            return None
        return self.save(new_filepath)

    def split(self, orientation: Qt.Orientation):
        """
        Splits the widget.

        :param orientation: The orientation of the split.
        """
        if self.splitted:
            return

        if self.splitter.orientation() != orientation:
            self.splitter.setOrientation(orientation)

        self._editor_2.show()
        self.splitted = True

    def undo_split(self):
        """
        Undo splits the widget.
        """
        if not self.splitted:
            return

        self._editor_2.hide()
        self.splitted = False

    @Slot(bool)
    def set_modified(self, modified: bool):
        """
        Sets the modified state of the widget.

        :param modified: The new modified state.
        """
        self.modified = modified
        self.tab_text_modified.emit(self.filepath, modified)
