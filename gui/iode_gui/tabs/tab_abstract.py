from PySide6.QtCore import Qt, QRect, Slot, Signal, QDir
from PySide6.QtWidgets import QWidget, QSplitter, QMessageBox

from iode_gui.utils import Context

from typing import List
from pathlib import Path
from iode import IodeType, IodeFileType, FileType, IODE_FILE_TYPES
from iode.util import check_filepath

class IodeAbstractWidget(QWidget):
    """
    Abstract base class for IODE widgets.
    """

    maybe_new_file_created = Signal(str)

    def __init__(self, file_type: IodeFileType, parent=None):
        """
        Initializes the IodeAbstractWidget.

        :param file_type: The type of the file.
        :param parent: The parent widget.
        """
        super().__init__(parent)
        self.file_type: IodeFileType = file_type
        self.tab_prefix: List[str] = []
        self.modified: bool = False
        self.forced_as_text: bool = False
        self.saving_file: bool = False
        self.splitted: bool = False
        self.splitter: QSplitter = None
        self.file_filter: str = ""
        self.project_dir: QDir = QDir.home()

        self.setGeometry(QRect(10, 11, 951, 26))

        # build tab_prefix list
        file_type: FileType = None
        for i, file_type in enumerate(IODE_FILE_TYPES):
            if file_type.extensions:
                ext = file_type.extensions[0].replace(".", "").upper()
                if i < len(IodeType):
                    ext = "&" + ext
                self.tab_prefix.append("(" + ext + ") ")
            else:
                self.tab_prefix.append("")

    @property
    def split_orientation(self) -> Qt.Orientation:
        """
        Returns the orientation of the splitter.

        :return: The orientation of the splitter.
        """
        return self.splitter.orientation()

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

    @filepath.setter
    def filepath(self, filepath: str):
        """
        Updates the filepath.

        :param filepath: The new filepath.
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
        if self.modified:
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
        self.filepath = filepath
        return success

    def _save_as(self) -> str:
        raise NotImplementedError()

    def save_as(self) -> str:
        """
        Prompt the user to save the current tab's content to a new file.

        Returns:
            str: The new filepath if the save was successful, otherwise an empty string.
        """
        new_filepath = self._save_as()

        if not new_filepath:
            return ""

        self.filepath = new_filepath
        self.maybe_new_file_created.emit(new_filepath)
        return new_filepath

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

    def update_colors(self):
        """
        Update the colors of the current tab.
        This method should be overridden in subclasses to apply specific color themes.
        """
        pass

    def _check_new_filepath(self, filepath: str) -> bool:
        try:
            check_filepath(filepath, self.file_type, file_must_exist=True)
        except Exception as e:
            QMessageBox.warning(None, "WARNING", str(e))
            return False
        return True

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

    def undo_split(self):
        """
        Undo split the current tab.
        """
        raise NotImplementedError()

    @Slot(bool)
    def set_modified(self, modified: bool):
        """Set the modified status of the current tab.

        Args:
            modified (bool): The new modified status.
        """
        raise NotImplementedError()




