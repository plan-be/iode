#include "file_print_preferences.h"


QIodeMenuFilePrintPref::QIodeMenuFilePrintPref(const QString& settings_filepath, QWidget* parent, Qt::WindowFlags f) :
    QIodeSettings(settings_filepath, parent, f)
{
    setupUi(this);

    QList<QString> q_colors;
    for(const std::string& color: vGraphsColors) q_colors << QString::fromStdString(color);
    comboBox_A2M_graph_background_color->addItems(q_colors);

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

QIodeMenuFilePrintPref::~QIodeMenuFilePrintPref()
{

}

void QIodeMenuFilePrintPref::set_printer_preferences()
{
    bool black_white = checkBox_black_white->isChecked();
    bool paragraph_numbering = checkBox_para_numbering->isChecked();
    int fontsize = spinBox_font_size->value();
    int fontincr = spinBox_font_incr->value();
    std::string fontfamily = fontComboBox_font_family->currentText().toStdString();

    bool table_color = checkBox_table_color->isChecked();
    bool table_shading = checkBox_table_shading->isChecked();
    bool table_break = checkBox_table_break->isChecked();
    bool table_new_page = checkBox_table_new_page->isChecked();
    bool graph_new_page = checkBox_graph_new_page->isChecked();

    int table_fontsize = spinBox_font_size_table->value();
    std::string table_fontfamily = fontComboBox_table_font_family->currentText().toStdString();
    int table_border_width = spinBox_table_border_width->value();

    int left_margin = spinBox_margin_left->value();
    int right_margin = spinBox_margin_right->value();
    int top_margin = spinBox_margin_top->value();
    int bottom_margin = spinBox_margin_bottom->value();

    bool prompt_printer = checkBox_prompt_printer->isChecked();
    std::string page_header = lineEdit_page_header->text().toStdString();
    std::string page_footer = lineEdit_page_footer->text().toStdString();
}

void QIodeMenuFilePrintPref::set_A2M_preferences()
{
    char esc_ch = lineEdit_A2M_escape_character->text().toStdString()[0];
    char sep_ch = lineEdit_A2M_cell_separator->text().toStdString()[0];
    char def_ch = lineEdit_A2M_define_character->text().toStdString()[0];
    char cmd_ch = lineEdit_A2M_command_character->text().toStdString()[0];

    std::string default_paragraph = lineEdit_A2M_default_paragraph->text().toStdString();
    bool preserve_spaces = checkBox_A2M_preserve_spaces->isChecked();
    bool preserve_linefeed = checkBox_A2M_preserve_linefeed->isChecked();
    bool append_to_existing_file = checkBox_A2M_append_to_existing_file->isChecked();

    int graph_width = spinBox_A2M_graph_width->value();
    int graph_height = spinBox_A2M_graph_height->value();
    int graph_backcolor = comboBox_A2M_graph_background_color->currentIndex();
    int graph_backbrush = spinBox_A2M_graph_background_brush->value();
    int graph_boxwidth = spinBox_A2M_graph_box->value();
}

void QIodeMenuFilePrintPref::set_MIF_preferences()
{
    int fontsize = spinBox_MIF_font_size->value();
    int fontincr = spinBox_MIF_font_incr->value();
    std::string fontfamily = fontComboBox_MIF_font_family->currentText().toStdString();

    bool table_color = checkBox_MIF_table_use_color->isChecked();
    bool table_split = checkBox_MIF_table_split_tables->isChecked();
    bool table_outside_borders = checkBox_MIF_table_outside_borders->isChecked();
    bool table_hlines = checkBox_MIF_table_horizontal_lines->isChecked();
    bool table_vlines = checkBox_MIF_table_vertical_lines->isChecked();

    int table_width = spinBox_MIF_table_width->value();
    int table_col1_width = spinBox_MIF_table_first_col_width->value();
    int table_coln_width = spinBox_MIF_table_other_col_width->value();
    int table_fontsize = spinBox_MIF_table_font_size->value();
    std::string table_fontfamily = fontComboBox_MIF_table_font_family->currentText().toStdString();

    bool image_refs = checkBox_MIF_image_ref_in_text->isChecked();
}

void QIodeMenuFilePrintPref::set_RTF_preferences()
{
    bool paragraph_numbers = checkBox_RTF_paragraph_numbers->isChecked();
    int fontsize = spinBox_RTF_font_size->value();
    int fontincr = spinBox_RTF_font_incr->value();
    std::string fontfamily = fontComboBox_RTF_font_family->currentText().toStdString();

    bool table_color = checkBox_RTF_table_use_color->isChecked();
    bool table_split = checkBox_RTF_table_split_tables->isChecked();
    bool table_outside_borders = checkBox_RTF_table_outside_borders->isChecked();
    bool table_hlines = checkBox_RTF_table_horizontal_lines->isChecked();
    bool table_vlines = checkBox_RTF_table_vertical_lines->isChecked();

    int table_width = spinBox_RTF_table_width->value();
    int table_col1_width = spinBox_RTF_table_first_col_width->value();
    int table_coln_width = spinBox_RTF_table_other_col_width->value();
    int table_fontsize = spinBox_RTF_table_font_size->value();
    std::string table_fontfamily = fontComboBox_RTF_table_font_family->currentText().toStdString();

    std::string help_title = lineEdit_RTF_help_title->text().toStdString();
    std::string copyright = lineEdit_RTF_copyright->text().toStdString();
    bool prepare_HCW = checkBox_RTF_prepare_for_HCW->isChecked();
    bool compress_help = checkBox_RTF_compress_help->isChecked();
    bool toc = checkBox_RTF_generate_TOC->isChecked();
}

void QIodeMenuFilePrintPref::set_HTML_preferences()
{
    int fontsize = spinBox_HTML_font_size->value();
    int fontincr = spinBox_HTML_font_incr->value();
    std::string fontfamily = fontComboBox_HTML_font_family->currentText().toStdString();

    bool table_color = checkBox_HTML_table_use_color->isChecked();
    int table_border_width = spinBox_HTML_table_border_width->value();
    int table_fontsize = spinBox_HTML_table_font_size->value();
    std::string table_fontfamily = fontComboBox_HTML_table_font_family->currentText().toStdString();

    bool paragraph_numbers = checkBox_HTML_paragraph_numbers->isChecked();
    bool toc = checkBox_HTML_generate_TOC->isChecked();
    std::string body_tag = textEdit_HTML_body_tag->toPlainText().toStdString();
    std::string title = lineEdit_HTML_title->text().toStdString();
}

void QIodeMenuFilePrintPref::set_print_preferences()
{
    try
    {
        void set_printer_preferences();
        void set_A2M_preferences();
        void set_MIF_preferences();
        void set_RTF_preferences();
        void set_HTML_preferences();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("Warning"), tr(e.what()));
    }

    this->accept();
}

void QIodeMenuFilePrintPref::help()
{
    QUrl url = get_url_iode_manual();
    QDesktopServices::openUrl(url);
}
