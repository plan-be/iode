from qtpy.QtCore import Slot
from qtpy.QtWidgets import QWidget, QGridLayout, QLayoutItem

from iode_gui.settings import MixinSettingsDialog
from iode_gui.util.widgets.file_chooser import EnumFileMode
from iode_gui.print.file_print_preferences import FilePrintPreferences
from .ui_print_file_dialog import Ui_PrintFileDialog

from typing import Union, Tuple
from pathlib import Path
from iode import (PRINT_FORMATS, IodeType, IodeFileType, FileType,  
                  PrintEquationsAs, PrintEquationsLecAs, PrintTablesAs)
from iode.common import (IODE_FILE_TYPES, PRINT_DEF_TABLES, PRINT_DEF_EQUATIONS, 
                         PRINT_DEF_EQ_LEC)


class PrintFileDialog(MixinSettingsDialog):
    def __init__(self, iode_type: IodeType=None, output_file: Union[str, Path]=None, parent: QWidget=None) -> None:
        super().__init__(parent)
        self.ui = Ui_PrintFileDialog()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)
        if iode_type is not None:
            self.menu_class_name += f"_{iode_type.name.upper()}"

        self.iode_type = iode_type

        self.v_file_formats = "ARHMCD"
        self.dict_file_formats = {'A': IodeFileType.FILE_A2M, 
                                  'R': IodeFileType.FILE_RTF,
                                  'H': IodeFileType.FILE_HTML,
                                  'M': IodeFileType.FILE_MIF,
                                  'C': IodeFileType.FILE_CSV, 
                                  'D': IodeFileType.FILE_ANY}
        self.ui.comboBox_print_format.addItems(PRINT_FORMATS)

        layout: QGridLayout = self.layout()

        row, col = self._find_item_position(self.ui.lineEdit_generalized_sample)
        self._horizontalSpacer_generalized_sample = layout.itemAtPosition(row, col + 1)
        row, col = self._find_item_position(self.ui.spinBox_nb_decimals)
        self._horizontalSpacer_nb_decimals = layout.itemAtPosition(row, col + 1)
        row, col = self._find_item_position(self.ui.comboBox_print_eq_as)
        self._horizontalSpacer_print_eq_as = layout.itemAtPosition(row, col + 1)
        row, col = self._find_item_position(self.ui.comboBox_print_eq_lec_as)
        self._horizontalSpacer_print_eq_lec_as = layout.itemAtPosition(row, col + 1)
        row, col = self._find_item_position(self.ui.comboBox_print_table_as)
        self._horizontalSpacer_print_table_as = layout.itemAtPosition(row, col + 1)

        self._hide_generalized_sample()
        if self.iode_type not in [IodeType.EQUATIONS, IodeType.SCALARS]:
            self._hide_nb_decimals()

        if self.iode_type == IodeType.EQUATIONS:
            self.v_print_eq_as = list(PrintEquationsAs)
            self.ui.comboBox_print_eq_as.addItems(PRINT_DEF_EQUATIONS)

            self.v_print_eq_lec_as = list(PrintEquationsLecAs)
            self.ui.comboBox_print_eq_lec_as.addItems(PRINT_DEF_EQ_LEC)
        else:
            self.ui.label_print_eq_as.hide()
            self.ui.comboBox_print_eq_as.hide()
            layout.removeItem(self._horizontalSpacer_print_eq_as)

            self.ui.label_print_eq_lec_as.hide()
            self.ui.comboBox_print_eq_lec_as.hide()
            layout.removeItem(self._horizontalSpacer_print_eq_lec_as)

        if self.iode_type == IodeType.TABLES:
            self.v_print_table_as = list(PrintTablesAs)
            self.ui.comboBox_print_table_as.addItems(PRINT_DEF_TABLES)
            self.ui.comboBox_print_table_as.currentIndexChanged.connect(self.show_options_for_computed_tables)
        else:
            self.ui.label_print_table_as.hide()
            self.ui.comboBox_print_table_as.hide()
            layout.removeItem(self._horizontalSpacer_print_table_as)

        self.ui.fileChooser_output_file.enum_file_type = IodeFileType.FILE_A2M
        self.ui.fileChooser_output_file.enum_file_mode = EnumFileMode.FILE_MAY_EXIST

        self.ui.comboBox_print_format.currentIndexChanged.connect(self.update_output_file_format)

        self.load_settings()

        # no output filepath loaded from the last saved settings
        if not self.output_file and output_file is not None:
            self.output_file = output_file

        if self.ui.comboBox_print_format.currentIndex() < 0:
            self.ui.comboBox_print_format.setCurrentIndex(0)
        else:
            self.update_output_file_format(-1)
        
        if self.iode_type == IodeType.EQUATIONS:
            if self.ui.comboBox_print_eq_as.currentIndex() < 0:
                self.ui.comboBox_print_eq_as.setCurrentIndex(0)
            if self.ui.comboBox_print_eq_lec_as.currentIndex() < 0:
                self.ui.comboBox_print_eq_lec_as.setCurrentIndex(0)
        
        if self.iode_type == IodeType.TABLES:
            if self.ui.comboBox_print_table_as.currentIndex() < 0:
                self.ui.comboBox_print_table_as.setCurrentIndex(0)
        
        # Adjust the size of the dialog
        self.adjustSize()


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

    @property
    def print_equations_as(self) -> PrintEquationsAs:
        if self.iode_type == IodeType.EQUATIONS:
            index = self.ui.comboBox_print_eq_as.currentIndex()
            value: PrintEquationsAs = self.v_print_eq_as[index]
            return value
        else:
            return None
    
    @property
    def print_equations_lec_as(self) -> PrintEquationsLecAs:
        if self.iode_type == IodeType.EQUATIONS:
            index = self.ui.comboBox_print_eq_lec_as.currentIndex()
            value: PrintEquationsLecAs = self.v_print_eq_lec_as[index]
            return value
        else:
            return None
    
    @property
    def print_tables_as(self) -> PrintTablesAs:
        if self.iode_type == IodeType.TABLES:
            index = self.ui.comboBox_print_table_as.currentIndex()
            value: PrintTablesAs = self.v_print_table_as[index]
            return value
        else:
            return None

    @property
    def generalized_sample(self) -> str:
        return self.ui.lineEdit_generalized_sample.text().strip()
    
    @property
    def print_nb_decimals(self) -> int:
        return self.ui.spinBox_nb_decimals.value()

    @Slot(int)
    def update_output_file_format(self, index: int):
        iode_file_type: IodeFileType = self.dict_file_formats[self.file_format]
        self.ui.fileChooser_output_file.enum_file_type = iode_file_type

        if iode_file_type != IodeFileType.FILE_ANY:
            output_file = self.output_file
            if not output_file:
                return
            output_file = Path(self.output_file).resolve()
            if not output_file.name:
                return
            file_type: FileType = IODE_FILE_TYPES[int(iode_file_type)]
            extension = file_type.extensions[0]
            output_file = output_file.with_suffix(extension)
            self.output_file = output_file

    def _find_item_position(self, item: QLayoutItem) -> Tuple[int, int]:
        layout: QGridLayout = self.layout()
        for row in range(layout.rowCount()):
            for col in range(layout.columnCount()):
                if layout.itemAtPosition(row, col) == item:
                    return row, col
        return -1, -1

    def _hide_generalized_sample(self):
        layout: QGridLayout = self.layout()
        self.ui.label_generalized_sample.hide()
        self.ui.lineEdit_generalized_sample.hide()
        layout.removeItem(self._horizontalSpacer_generalized_sample)

    def _show_generalized_sample(self):
        layout: QGridLayout = self.layout()
        self.ui.label_generalized_sample.show()
        self.ui.lineEdit_generalized_sample.show()
        row, col = self._find_item_position(self.ui.lineEdit_generalized_sample)
        if row >= 0 and col >= 0:
            layout.addItem(self._horizontalSpacer_generalized_sample, row , col + 1)

    def _hide_nb_decimals(self):
        layout: QGridLayout = self.layout()
        self.ui.label_nb_decimals.hide()
        self.ui.spinBox_nb_decimals.hide()
        layout.removeItem(self._horizontalSpacer_nb_decimals)
 
    def _show_nb_decimals(self):
        layout: QGridLayout = self.layout()
        self.ui.label_nb_decimals.show()
        self.ui.spinBox_nb_decimals.show()
        row, col = self._find_item_position(self.ui.spinBox_nb_decimals)
        if row >= 0 and col >= 0:
            layout.addItem(self._horizontalSpacer_nb_decimals, row , col + 1)

    @Slot(int)
    def show_options_for_computed_tables(self, index: int):
        if self.iode_type == IodeType.TABLES:
            if self.print_tables_as == PrintTablesAs.COMPUTED:
                self._show_generalized_sample()
                self._show_nb_decimals()
            else:
                self._hide_generalized_sample()
                self._hide_nb_decimals()
            # Adjust the size of the dialog
            self.adjustSize()

    # Will automatically save to settings -> see IodeSettingsDialog::closeEvent()
    @Slot()
    def apply(self):
        self.accept()

    @Slot()
    def set_print_options(self):
        dialog = FilePrintPreferences(self.parent())
        dialog.exec()
