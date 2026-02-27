from qtpy.QtCore import Slot
from qtpy.QtWidgets import QMessageBox

from iode_gui.settings import MixinSettingsDialog
from .ui_file_print_preferences import Ui_FilePrintPreferences

from iode import IODE_FONTS, GRAPHS_COLORS
from iode.util import (set_printer_preferences, set_A2M_preferences, set_MIF_preferences, 
                       set_RTF_preferences, set_HTML_preferences)


class FilePrintPreferences(MixinSettingsDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_FilePrintPreferences()
        self.ui.setupUi(self)
        self.prepare_settings(self.ui)

        self.ui.comboBox_font_family.addItems(IODE_FONTS)
        self.ui.comboBox_table_font_family.addItems(IODE_FONTS)

        self.ui.comboBox_A2M_graph_background_color.addItems(GRAPHS_COLORS)

        self.ui.comboBox_MIF_font_family.addItems(IODE_FONTS)
        self.ui.comboBox_MIF_table_font_family.addItems(IODE_FONTS)

        self.ui.comboBox_RTF_font_family.addItems(IODE_FONTS)
        self.ui.comboBox_RTF_table_font_family.addItems(IODE_FONTS)

        self.ui.comboBox_HTML_font_family.addItems(IODE_FONTS)
        self.ui.comboBox_HTML_table_font_family.addItems(IODE_FONTS)

        self.load_settings()

        # set_printer_preferences
        if not self.ui.spinBox_font_size.value():
            self.ui.spinBox_font_size.setValue(10)
        if not self.ui.spinBox_font_incr.value():
            self.ui.spinBox_font_incr.setValue(2)
        if not self.ui.spinBox_table_font_size.value():
            self.ui.spinBox_table_font_size.setValue(10)
        if not self.ui.spinBox_margin_left.value():
            self.ui.spinBox_margin_left.setValue(10)
        if not self.ui.spinBox_margin_right.value():
            self.ui.spinBox_margin_right.setValue(10)
        if not self.ui.spinBox_margin_top.value():
            self.ui.spinBox_margin_top.setValue(10)
        if not self.ui.spinBox_margin_bottom.value():
            self.ui.spinBox_margin_bottom.setValue(10)
        if not self.ui.checkBox_prompt_printer.isChecked():
            self.ui.checkBox_prompt_printer.setChecked(True)

        # set_A2M_preferences
        if not self.ui.lineEdit_A2M_escape_character.text():
            self.ui.lineEdit_A2M_escape_character.setText('\\')
        if not self.ui.lineEdit_A2M_cell_separator.text():
            self.ui.lineEdit_A2M_cell_separator.setText('@')
        if not self.ui.lineEdit_A2M_define_character.text():
            self.ui.lineEdit_A2M_define_character.setText('&')
        if not self.ui.lineEdit_A2M_command_character.text():
            self.ui.lineEdit_A2M_command_character.setText('.')
        if not self.ui.spinBox_A2M_graph_width.value():
            self.ui.spinBox_A2M_graph_width.setValue(160)
        if not self.ui.spinBox_A2M_graph_height.value():
            self.ui.spinBox_A2M_graph_height.setValue(100)
        if not self.ui.spinBox_A2M_graph_background_brush.value():
            self.ui.spinBox_A2M_graph_background_brush.setValue(1)

        # set_MIF_preferences
        if not self.ui.spinBox_MIF_font_size.value():
            self.ui.spinBox_MIF_font_size.setValue(10)
        if not self.ui.spinBox_MIF_font_incr.value():
            self.ui.spinBox_MIF_font_incr.setValue(2)
        if not self.ui.spinBox_MIF_table_font_size.value():
            self.ui.spinBox_MIF_table_font_size.setValue(10)
        if not self.ui.spinBox_MIF_table_first_col_width.value():
            self.ui.spinBox_MIF_table_first_col_width.setValue(60)
        if not self.ui.spinBox_MIF_table_other_col_width.value():
            self.ui.spinBox_MIF_table_other_col_width.setValue(15)
        if not self.ui.spinBox_MIF_table_width.value():
            self.ui.spinBox_MIF_table_width.setValue(165)
        
        # set_MIF_preferences
        if not self.ui.spinBox_RTF_font_size.value():
            self.ui.spinBox_RTF_font_size.setValue(10)
        if not self.ui.spinBox_RTF_font_incr.value():
            self.ui.spinBox_RTF_font_incr.setValue(2)
        if not self.ui.spinBox_RTF_table_width.value():
            self.ui.spinBox_RTF_table_width.setValue(165)
        if not self.ui.spinBox_RTF_table_first_col_width.value():
            self.ui.spinBox_RTF_table_first_col_width.setValue(60)
        if not self.ui.spinBox_RTF_table_other_col_width.value():
            self.ui.spinBox_RTF_table_other_col_width.setValue(15)
        if not self.ui.spinBox_RTF_table_font_size.value():
            self.ui.spinBox_RTF_table_font_size.setValue(10)

        # set_HTML_preferences
        if not self.ui.spinBox_HTML_font_size.value():
            self.ui.spinBox_HTML_font_size.setValue(10)
        if not self.ui.spinBox_HTML_font_incr.value():
            self.ui.spinBox_HTML_font_incr.setValue(2)
        if not self.ui.spinBox_HTML_table_font_size.value():
            self.ui.spinBox_HTML_table_font_size.setValue(10)

    def _set_printer_preferences(self):
        font_size = self.ui.spinBox_font_size.value()
        font_family = self.ui.comboBox_font_family.currentText()
        table_font_family = self.ui.comboBox_table_font_family.currentText()
        font_incr = self.ui.spinBox_font_incr.value()
        table_font_size = self.ui.spinBox_table_font_size.value()
        paragraph_numbers = self.ui.checkBox_para_numbering.isChecked()
        left_margin = self.ui.spinBox_margin_left.value()
        right_margin = self.ui.spinBox_margin_right.value()
        top_margin = self.ui.spinBox_margin_top.value()
        bottom_margin = self.ui.spinBox_margin_bottom.value()
        table_border_width = self.ui.spinBox_table_border_width.value()
        black_and_white = self.ui.checkBox_black_white.isChecked()
        table_break = self.ui.checkBox_table_break.isChecked()
        table_new_page = self.ui.checkBox_table_new_page.isChecked()
        graph_new_page = self.ui.checkBox_graph_new_page.isChecked()
        table_shading = self.ui.checkBox_table_shading.isChecked()
        page_header = self.ui.lineEdit_page_header.text()
        page_footer = self.ui.lineEdit_page_footer.text()
        ask_prompt = self.ui.checkBox_prompt_printer.isChecked()

        set_printer_preferences(font_size, font_family, table_font_family, font_incr, 
                                table_font_size, paragraph_numbers, left_margin, 
                                right_margin, top_margin, bottom_margin, black_and_white, 
                                table_border_width, table_break, table_new_page, 
                                graph_new_page, table_shading, page_header, page_footer, 
                                ask_prompt)

    def _set_A2M_preferences(self):
        escape_char = self.ui.lineEdit_A2M_escape_character.text()
        cell_separator = self.ui.lineEdit_A2M_cell_separator.text()
        define_char = self.ui.lineEdit_A2M_define_character.text()
        command_char = self.ui.lineEdit_A2M_command_character.text()
        append = self.ui.checkBox_A2M_append_to_existing_file.isChecked()
        preserve_spaces = self.ui.checkBox_A2M_preserve_spaces.isChecked()
        preserve_linefeed = self.ui.checkBox_A2M_preserve_linefeed.isChecked()
        default_paragraph = self.ui.lineEdit_A2M_default_paragraph.text()
        graph_width = self.ui.spinBox_A2M_graph_width.value()
        graph_height = self.ui.spinBox_A2M_graph_height.value()
        graph_background_color = self.ui.comboBox_A2M_graph_background_color.currentText()
        graph_background_brush = self.ui.spinBox_A2M_graph_background_brush.value()
        graph_box = self.ui.spinBox_A2M_graph_box.value()

        set_A2M_preferences(escape_char, cell_separator, define_char, command_char, 
                            append, preserve_spaces, preserve_linefeed, default_paragraph, 
                            graph_width, graph_height, graph_background_color, 
                            graph_background_brush, graph_box)

    def _set_MIF_preferences(self):
        font_size = self.ui.spinBox_MIF_font_size.value()
        font_incr = self.ui.spinBox_MIF_font_incr.value()
        font_family = self.ui.comboBox_MIF_font_family.currentText()
        table_font_family = self.ui.comboBox_MIF_table_font_family.currentText()
        table_font_size = self.ui.spinBox_MIF_table_font_size.value()
        table_use_color = self.ui.checkBox_MIF_table_use_color.isChecked()
        table_first_col_width = self.ui.spinBox_MIF_table_first_col_width.value()
        table_other_col_width = self.ui.spinBox_MIF_table_other_col_width.value()
        table_split = self.ui.checkBox_MIF_table_split_tables.isChecked()
        table_width = self.ui.spinBox_MIF_table_width.value()
        table_outside_borders = self.ui.checkBox_MIF_table_outside_borders.isChecked()
        table_horizontal_lines = self.ui.checkBox_MIF_table_horizontal_lines.isChecked()
        table_vertical_lines = self.ui.checkBox_MIF_table_vertical_lines.isChecked()
        image_ref_in_text = self.ui.checkBox_MIF_image_ref_in_text.isChecked()

        set_MIF_preferences(font_size, font_incr, font_family, table_font_family, 
                            table_font_size, table_use_color, table_first_col_width, 
                            table_other_col_width, table_split, table_width, 
                            table_outside_borders, table_horizontal_lines, 
                            table_vertical_lines, image_ref_in_text)

    def _set_RTF_preferences(self):
        paragraph_numbers = self.ui.checkBox_RTF_paragraph_numbers.isChecked()
        font_size = self.ui.spinBox_RTF_font_size.value()
        font_incr = self.ui.spinBox_RTF_font_incr.value()
        font_family = self.ui.comboBox_RTF_font_family.currentText()
        table_use_color = self.ui.checkBox_RTF_table_use_color.isChecked()
        table_split_tables = self.ui.checkBox_RTF_table_split_tables.isChecked()
        table_outside_borders = self.ui.checkBox_RTF_table_outside_borders.isChecked()
        table_vertical_lines = self.ui.checkBox_RTF_table_vertical_lines.isChecked()
        table_horizontal_lines = self.ui.checkBox_RTF_table_horizontal_lines.isChecked()
        table_width = self.ui.spinBox_RTF_table_width.value()
        table_first_col_width = self.ui.spinBox_RTF_table_first_col_width.value()
        table_other_col_width = self.ui.spinBox_RTF_table_other_col_width.value()
        table_font_size = self.ui.spinBox_RTF_table_font_size.value()
        table_font_family = self.ui.comboBox_RTF_table_font_family.currentText()
        help_title = self.ui.lineEdit_RTF_help_title.text()
        copyright = self.ui.lineEdit_RTF_copyright.text()
        prepare_for_hcw = self.ui.checkBox_RTF_prepare_for_HCW.isChecked()
        compress_help = self.ui.checkBox_RTF_compress_help.isChecked()
        generate_toc = self.ui.checkBox_RTF_generate_TOC.isChecked()

        set_RTF_preferences(font_size, font_incr, font_family, table_font_family, 
                            table_font_size, paragraph_numbers, table_use_color, 
                            table_first_col_width, table_other_col_width, 
                            table_width, table_outside_borders, table_horizontal_lines, 
                            table_vertical_lines, table_split_tables, prepare_for_hcw, 
                            compress_help, generate_toc, help_title, copyright)

    def _set_HTML_preferences(self):
        font_size = self.ui.spinBox_HTML_font_size.value()
        font_incr = self.ui.spinBox_HTML_font_incr.value()
        font_family = self.ui.comboBox_HTML_font_family.currentText()
        table_use_color = self.ui.checkBox_HTML_table_use_color.isChecked()
        table_border_width = self.ui.spinBox_HTML_table_border_width.value()
        table_font_size = self.ui.spinBox_HTML_table_font_size.value()
        table_font_family = self.ui.comboBox_HTML_table_font_family.currentText()
        paragraph_numbers = self.ui.checkBox_HTML_paragraph_numbers.isChecked()
        generate_toc = self.ui.checkBox_HTML_generate_TOC.isChecked()
        body_tag = self.ui.textEdit_HTML_body_tag.toPlainText()
        title = self.ui.lineEdit_HTML_title.text()

        set_HTML_preferences(font_size, font_family, table_font_family, font_incr, 
                             table_font_size, paragraph_numbers, table_border_width, 
                             table_use_color, generate_toc, body_tag, title)

    @Slot()
    def set_print_preferences(self):
        try:
            self._set_printer_preferences()
            self._set_A2M_preferences()
            self._set_MIF_preferences()
            self._set_RTF_preferences()
            self._set_HTML_preferences()

            self.accept()
        except Exception as e:
            QMessageBox.warning(self, "WARNING", str(e))
