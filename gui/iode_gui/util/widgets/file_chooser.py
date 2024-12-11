from PySide6.QtWidgets import QWidget, QHBoxLayout, QLineEdit, QPushButton, QFileDialog
from PySide6.QtCore import QDir, QFileInfo

from enum import Enum, auto
from pathlib import Path
from typing import Union
from iode import IodeFileType, FileType, IODE_FILE_TYPES


class EnumFileMode(Enum):
    NEW_FILE = auto()
    EXISTING_FILE = auto()
    FILE_MAY_EXIST = auto()


class IodeFileChooser(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.lineEdit = QLineEdit()
        self.browseButton = QPushButton()
        self._enum_file_type = IodeFileType.FILE_ANY
        self._enum_file_mode = EnumFileMode.EXISTING_FILE

        layout = QHBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(1)

        self.lineEdit.setPlaceholderText("*.*")
        layout.addWidget(self.lineEdit)

        self.browseButton.setText("Browse")
        self.browseButton.setFixedWidth(self.browseButton.fontMetrics().\
                                        horizontalAdvance("  " + self.browseButton.text() + "  "))
        layout.addWidget(self.browseButton)

        self.setFocusProxy(self.lineEdit)

        self.browseButton.clicked.connect(self.browse)

        # There is no QFileDialog::getSaveDirectory().
        # Only QFileDialog::getExistingDirectory
        if self.enum_file_type == IodeFileType.DIRECTORY:
            self.enum_file_mode = EnumFileMode.EXISTING_FILE

    @property
    def enum_file_type(self) -> IodeFileType:
        return self._enum_file_type
    
    @enum_file_type.setter
    def enum_file_type(self, enum_file_type: IodeFileType):
        if not isinstance(enum_file_type, IodeFileType):
            raise TypeError("'enum_file_type' must be an instance of IodeFileType")
        self._enum_file_type = enum_file_type

    @property
    def enum_file_mode(self) -> EnumFileMode:
        return self._enum_file_mode
    
    @enum_file_mode.setter
    def enum_file_mode(self, enum_file_mode: EnumFileMode):
        if not isinstance(enum_file_mode, EnumFileMode):
            raise TypeError("'enum_file_mode' must be an instance of EnumFileMode")
        self._enum_file_mode = enum_file_mode

    @property
    def filepath(self) -> str:
        return self.lineEdit.text()

    @filepath.setter
    def filepath(self, filepath: Union[str, Path]):
        if not isinstance(filepath, (str, Path)):
            raise TypeError("'filepath' must be an instance of str or Path")
        if isinstance(filepath, Path):
            filepath = str(filepath.resolve())
        self.lineEdit.setText(filepath)

    def browse(self):
        file_type: FileType = IODE_FILE_TYPES[self.enum_file_type]
        path = self.lineEdit.text()
        name = file_type.name

        filter = ""
        if self.enum_file_type != IodeFileType.FILE_ANY:
            extensions = [f"*{ext}" for ext in file_type.extensions]
            filter = f"{name} ({', '.join(extensions)})"

        if not path:
            rootDir = QDir.homePath()
        else:
            parentDir = QFileInfo(path).absoluteDir()
            rootDir = parentDir.absolutePath() if parentDir.exists() else QDir.homePath()

        caption = name + " File"
        if self.enum_file_mode == EnumFileMode.EXISTING_FILE:
            if self.enum_file_type == IodeFileType.DIRECTORY:
                path = QFileDialog.getExistingDirectory(self, "Open Directory", 
                                                        rootDir, QFileDialog.Option.ShowDirsOnly)
            else:
                caption = "Open " + caption
                path = QFileDialog.getOpenFileName(self, caption, rootDir, filter)[0]
        else:
            caption = "Save " + caption
            path = QFileDialog.getSaveFileName(self, caption, rootDir, filter)[0]

        if path:
            self.lineEdit.setText(path)
