from PyQt6.QtWidgets import QWidget, QSplitter, QMessageBox
from PyQt6.QtCore import Qt, QRect, pyqtSlot

from iode import IodeFileType, IODE_FILE_TYPES
from iode.util import _check_filepath
from typing import List
from pathlib import Path


class IodeAbstractWidget(QWidget):
    """
    Abstract base class for IODE widgets.
    """

    def __init__(self, file_type: IodeFileType, parent=None):
        """
        Initializes the IodeAbstractWidget.

        :param file_type: The type of the file.
        :param parent: The parent widget.
        """
        super().__init__(parent)
        self._file_type: IodeFileType = file_type
        self.tab_prefix: List[str] = []
        self._modified: bool = False
        self._forced_as_text: bool = False
        self.saving_file: bool = False
        self._splitted: bool = False
        self._splitter: QSplitter = None

        self.setGeometry(QRect(10, 11, 951, 26))

        # build tab_prefix list
        for i, file_type in enumerate(IODE_FILE_TYPES):
            if file_type.extensions:
                ext = file_type.extensions[0][1:].upper()
                if i < IodeFileType.FILE_VARIABLES:
                    ext = "&" + ext
                self.tab_prefix.append("(" + ext + ") ")
            else:
                self.tab_prefix.append("")

    @property
    def file_type(self) -> IodeFileType:
        """
        Returns the file type.

        :return: The file type.
        """
        return self._file_type

    @property
    def modified(self) -> bool:
        """
        Returns whether the file has been modified.

        :return: True if the file has been modified, False otherwise.
        """
        return self._modified

    @property
    def forced_as_text(self) -> bool:
        """
        Returns whether the file has been forced to open as a text tab.

        :return: True if the file has been forced to open as a text tab, False otherwise.
        """
        return self._forced_as_text

    @forced_as_text.setter
    def forced_as_text(self, value: bool):
        """
        Sets whether the file has been forced to open as a text tab.

        :param value: True if the file has been forced to open as a text tab, False otherwise.
        """
        self._forced_as_text = value

    @property
    def splitted(self) -> bool:
        """
        Returns whether the tab has been split.

        :return: True if the tab has been split, False otherwise.
        """
        return self._splitted

    @property
    def split_orientation(self) -> Qt.Orientation:
        """
        Returns the orientation of the splitter.

        :return: The orientation of the splitter.
        """
        return self._splitter.orientation()

    @property
    def filepath(self) -> str:
        """
        Filepath of the file associated with the current tab.
        Returns None if the current tab is not associated with any file.

        Returns
        -------
        str
        """
        raise NotImplementedError()

    @property
    def tab_text(self) -> str:
        """
        Returns the text to be displayed on the tab.

        :return: The text to be displayed on the tab.
        """
        filename = Path(self.filepath).name
        text = self.tab_prefix[self.file_type] + filename
        if self._modified:
            text += "*"
        return text

    @property
    def tooltip(self) -> str:
        """
        Returns the tooltip for the tab.

        :return: The tooltip for the tab.
        """
        filepath = Path(self.filepath).absolute().resolve()
        return str(filepath) if filepath.exists() else ""

    def _load(self, filepath: str, force_overwrite: bool) -> bool:
        raise NotImplementedError()

    def load(self, filepath: str, force_overwrite: bool) -> bool:
        """
        Loads the file.

        :param filepath: The path to the file.
        :param force_overwrite: Whether to force overwrite the file.
        :return: True if the file was loaded successfully, False otherwise.
        """
        success = self._load(filepath, force_overwrite)
        if success:
            success = self.update_filepath(filepath)
        return success

    def _save_as(self) -> str:
        raise NotImplementedError()

    def save_as(self):
        """
        Prompt the user to save the current tab's content to a new file.

        Returns:
            str: The new filepath if the save was successful, otherwise an empty string.
        """
        new_filepath = self._save_as()

        if not new_filepath:
            return ""

        success = self.update_filepath(new_filepath)
        return new_filepath if success else ""

    def load_settings(self):
        """
        Load settings for the current tab from a QSettings object.
        """
        pass

    def save_settings(self):
        """
        Save settings for the current tab to a QSettings object.
        """
        pass

    def _check_new_filepath(self, filepath: str) -> bool:
        try:
            _check_filepath(filepath, self.file_type, "load", True)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return False
        return True

    def update_filepath(self, filepath: str) -> bool:
        """
        Update the filepath associated with the current tab.

        Args:
            filepath (str): The new filepath.

        Returns:
            bool: True if the update was successful, False otherwise.

        Note:
            This method needs to be called each time a file is moved or renamed.
        """
        raise NotImplementedError()

    def update(self):
        """
        Update the content of the current tab.
        """
        raise NotImplementedError()

    def save(self) -> str:
        """
        Save the content of the current tab.

        Returns:
            str: The filepath of the saved file.
        """
        raise NotImplementedError

    def split(self, orientation: Qt.Orientation):
        """
        Split the current tab into two tabs.

        Args:
            orientation (Qt.Orientation): The orientation of the split.
        """
        raise NotImplementedError()

    def unsplit(self):
        """
        Unsplit the current tab.
        """
        raise NotImplementedError()

    @pyqtSlot(bool)
    def set_modified(self, modified: bool):
        """Set the modified status of the current tab.

        Args:
            modified (bool): The new modified status.
        """
        raise NotImplementedError()




