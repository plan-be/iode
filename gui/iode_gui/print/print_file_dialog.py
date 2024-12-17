from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget

from iode_gui.settings import MixinSettingsDialog
from iode_gui.util.widgets.file_chooser import EnumFileMode
from iode_gui.print.file_print_preferences import FilePrintPreferences
from .ui_print_file_dialog import Ui_PrintFileDialog

from typing import Union
from pathlib import Path
from iode import PRINT_FORMATS, IodeFileType


class PrintFileDialog(MixinSettingsDialog):
    def __init__(self, output_file: Union[str, Path] = None, file_format: str = None, parent: QWidget = None) -> None:
        super().__init__(parent)
        self.ui = Ui_PrintFileDialog()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.v_file_formats = "ARHMCD"
        self.dict_file_formats = {'A': IodeFileType.FILE_A2M, 
                                  'R': IodeFileType.FILE_RTF,
                                  'H': IodeFileType.FILE_HTML,
                                  'M': IodeFileType.FILE_MIF,
                                  'C': IodeFileType.FILE_CSV,
                                  'D': IodeFileType.FILE_ANY}
        self.ui.comboBox_print_format.addItems(PRINT_FORMATS)
        self.ui.comboBox_print_format.setCurrentIndex(0)

        self.ui.fileChooser_output_file.enum_file_type = IodeFileType.FILE_A2M
        self.ui.fileChooser_output_file.enum_file_mode = EnumFileMode.FILE_MAY_EXIST

        self.ui.comboBox_print_format.currentIndexChanged.connect(self.update_output_file_format)

        self.load_settings()

        if output_file is not None:
            self.output_file = output_file
        if file_format is not None:
            self.file_format = file_format

    @property
    def file_format(self) -> str:
        index_format = self.ui.comboBox_print_format.currentIndex()
        return self.v_file_formats[index_format]
    
    @file_format.setter
    def file_format(self, value: str):
        if value is None or not len(value):
            raise ValueError("'file_format: empty value")
        if len(value) > 1:
            value = value[0]
        if value not in self.v_file_formats:
            raise ValueError(f"'file_format': invalid file format '{value}'. "
                             f"Expected one of '{self.v_file_formats}'")
        index_format = self.v_file_formats.index(value)
        self.ui.comboBox_print_format.setCurrentIndex(index_format)
    
    @property
    def output_file(self) -> str:
        return self.ui.fileChooser_output_file.filepath
    
    @output_file.setter
    def output_file(self, value: Union[str, Path]):
        if isinstance(value, Path):
            value = str(value.resolve())
        if value is None or not len(value):
            raise ValueError("'output_file': empty filepath")
        self.ui.fileChooser_output_file.filepath = value

    @Slot(int)
    def update_output_file_format(self, index: int):
        self.ui.fileChooser_output_file.enum_file_type = self.dict_file_formats[self.file_format]

    # Will automatically save to settings -> see IodeSettingsDialog::closeEvent()
    @Slot()
    def apply(self):
        self.accept()

    @Slot()
    def set_print_options(self):
        dialog = FilePrintPreferences(self.parent())
        dialog.exec()
