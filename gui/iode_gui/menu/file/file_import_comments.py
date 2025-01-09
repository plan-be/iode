from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget, QMessageBox

from iode_gui.settings import MixinSettingsDialog
from iode_gui.util.widgets.file_chooser import EnumFileMode
from .ui_file_import_comments import Ui_MenuFileImportComments

from iode import IodeFileType, TableLang #, IodeImportFormat, import_comments_from_file


class MenuFileImportComments(MixinSettingsDialog):
    def __init__(self, parent: QWidget = None) -> None:
        super().__init__(parent)
        self.ui = Ui_MenuFileImportComments()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        # self.v_import_formats = list(IodeExportFormat)
        # v_import_formats_names = [import_format.name.title() for import_format in self.v_import_formats]
        # self.ui.comboBox_format.addItems(v_import_formats_names)
        # self.ui.comboBox_format.setCurrentIndex(0) 

        self.v_table_langs = list(TableLang)
        v_table_lang_names = [iode_type.name.title() for iode_type in self.v_table_langs]
        self.ui.comboBox_language.addItems(v_table_lang_names)
        self.ui.comboBox_language.setCurrentIndex(0)

        self.ui.fileChooser_input_file.enum_file_type = IodeFileType.FILE_COMMENTS
        self.ui.fileChooser_input_file.enum_file_mode = EnumFileMode.EXISTING_FILE

        self.ui.fileChooser_rule_file.enum_file_type = IodeFileType.FILE_ANY
        self.ui.fileChooser_rule_file.enum_file_mode = EnumFileMode.EXISTING_FILE

        self.ui.fileChooser_save_file.enum_file_type = IodeFileType.FILE_COMMENTS
        self.ui.fileChooser_save_file.enum_file_mode = EnumFileMode.FILE_MAY_EXIST

        self.ui.fileChooser_debug_file.enum_file_type = IodeFileType.FILE_LOG
        self.ui.fileChooser_debug_file.enum_file_mode = EnumFileMode.FILE_MAY_EXIST

        self.load_settings()

    @Slot()
    def import_comments(self):
        try:
            input_file: str = self.ui.fileChooser_input_file.filepath.strip()
            rule_file: str = self.ui.fileChooser_rule_file.filepath.strip()
            save_file: str = self.ui.fileChooser_save_file.filepath.strip()
            debug_file: str = self.ui.fileChooser_debug_file.filepath.strip()

            if not input_file:
                raise ValueError("Please specify an input file.")

            if not save_file:
                raise ValueError("Please specify a save file path.")

            i_table_lang = self.ui.comboBox_language.currentIndex()
            table_lang: TableLang = self.v_table_langs[i_table_lang]

            # i_import_format = self.ui.comboBox_format.currentIndex()
            # import_format: IodeExportFormat = self.v_import_formats[i_import_format]

            # import_comments_from_file(input_file, import_format, save_file, rule_file, 
            #                           table_lang, debug_file)
            
            raise NotImplementedError("This feature is not yet implemented.")

            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", str(e))
