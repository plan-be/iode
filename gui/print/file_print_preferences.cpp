#include "file_print_preferences.h"


FilePrintPreferences::FilePrintPreferences(QWidget* parent) :
    IodeSettingsDialog(parent)
{
    setupUi(this);

    QList<QString> q_colors;
    for(const std::string& color: vGraphsColors) q_colors << QString::fromStdString(color);


    QList<QString> q_fonts;
    for(const std::string& font: vIodeFonts) q_fonts << QString::fromStdString(font);

    // default values
    A2M_GWIDTH = 9.5;
    A2M_GHEIGHT = 6.5;
    A2M_BOXWIDTH = 1;
    A2M_BACKBRUSH = 50;
    A2M_BACKCOLOR = 'b';

    // PRINTER tab
    wPrinterFontSize = new WrapperSpinBox(label_font_size->text(), *spinBox_font_size, REQUIRED_FIELD);
    wPrinterFontFamily = new WrapperComboBox(label_font_family->text(), *comboBox_font_family, REQUIRED_FIELD, q_fonts);
    wPrinterTableFontFamily = new WrapperComboBox(label_table_font_family->text(), *comboBox_table_font_family, REQUIRED_FIELD, q_fonts);
    wPrinterFontIncr = new WrapperSpinBox(label_font_incr->text(), *spinBox_font_incr, REQUIRED_FIELD);
    wPrinterTableFontSize = new WrapperSpinBox(label_table_font_size->text(), *spinBox_table_font_size, REQUIRED_FIELD);
    wPrinterParagraphNumbers = new WrapperCheckBox(checkBox_para_numbering->text(), *checkBox_para_numbering, REQUIRED_FIELD);
    wPrinterLeftMargin = new WrapperSpinBox(label_margin_left->text(), *spinBox_margin_left, REQUIRED_FIELD);
    wPrinterRightMargin = new WrapperSpinBox(label_margin_right->text(), *spinBox_margin_right, REQUIRED_FIELD);
    wPrinterTopMargin = new WrapperSpinBox(label_margin_top->text(), *spinBox_margin_top, REQUIRED_FIELD);
    wPrinterBottomMargin = new WrapperSpinBox(label_margin_bottom->text(), *spinBox_margin_bottom, REQUIRED_FIELD);
    wPrinterTableBorderWidth = new WrapperSpinBox(label_table_border_width->text(), *spinBox_table_border_width, REQUIRED_FIELD);
    wPrinterBlackAndWhite = new WrapperCheckBox(checkBox_black_white->text(), *checkBox_black_white, REQUIRED_FIELD);
    wPrinterTableBreak = new WrapperCheckBox(checkBox_table_break->text(), *checkBox_table_break, REQUIRED_FIELD);
    wPrinterTableNewPage = new WrapperCheckBox(checkBox_table_new_page->text(), *checkBox_table_new_page, REQUIRED_FIELD);
    wPrinterGraphNewPage = new WrapperCheckBox(checkBox_graph_new_page->text(), *checkBox_graph_new_page, REQUIRED_FIELD);
    wPrinterTableShading = new WrapperCheckBox(checkBox_table_shading->text(), *checkBox_table_shading, REQUIRED_FIELD);
    wPrinterPageHeader = new WrapperQLineEdit(label_page_header->text(), *lineEdit_page_header, OPTIONAL_FIELD);
    wPrinterPageFooter = new WrapperQLineEdit(label_page_footer->text(), *lineEdit_page_footer, OPTIONAL_FIELD);
    wPrinterPrompt = new WrapperCheckBox(checkBox_prompt_printer->text(), *checkBox_prompt_printer, REQUIRED_FIELD);

    comboBox_font_family->setCurrentIndex(0);
    comboBox_table_font_family->setCurrentIndex(0);

    mapFields["Printer_Font_Size"]          = wPrinterFontSize;
    mapFields["Printer_Font_Family"]        = wPrinterFontFamily;
    mapFields["Printer_Table_Font_Family"]  = wPrinterTableFontFamily;
    mapFields["Printer_Font_Incr"]          = wPrinterFontIncr;
    mapFields["Printer_Table_Font_Size"]    = wPrinterTableFontSize;
    mapFields["Printer_Paragraph_Numbers"]  = wPrinterParagraphNumbers;
    mapFields["Printer_Left_Margin"]        = wPrinterLeftMargin;
    mapFields["Printer_Right_Margin"]       = wPrinterRightMargin;
    mapFields["Printer_Top_Margin"]         = wPrinterTopMargin;
    mapFields["Printer_Bottom_Margin"]      = wPrinterBottomMargin;
    mapFields["Printer_Table_Border_Width"] = wPrinterTableBorderWidth;
    mapFields["Printer_Black_And_White"]    = wPrinterBlackAndWhite;
    mapFields["Printer_Table_Break"]        = wPrinterTableBreak;
    mapFields["Printer_Table_New_Page"]     = wPrinterTableNewPage;
    mapFields["Printer_Graph_New_Page"]     = wPrinterGraphNewPage;
    mapFields["Printer_Table_Shading"]      = wPrinterTableShading;
    mapFields["Printer_Page_Header"]        = wPrinterPageHeader;
    mapFields["Printer_Page_Footer"]        = wPrinterPageFooter;
    mapFields["Printer_Prompt"]             = wPrinterPrompt;

    // A2M tab
    wA2MEscapeChar = new WrapperQLineEdit(label_A2M_escape_character->text(), *lineEdit_A2M_escape_character, REQUIRED_FIELD);
    wA2MCellSeparator = new WrapperQLineEdit(label_A2M_cell_separator->text(), *lineEdit_A2M_cell_separator, REQUIRED_FIELD);
    wA2MDefineChar = new WrapperQLineEdit(label_A2M_define_character->text(), *lineEdit_A2M_define_character, REQUIRED_FIELD);
    wA2MCommandChar = new WrapperQLineEdit(label_A2M_command_character->text(), *lineEdit_A2M_command_character, REQUIRED_FIELD);
    wA2MAppend = new WrapperCheckBox(checkBox_A2M_append_to_existing_file->text(), *checkBox_A2M_append_to_existing_file, REQUIRED_FIELD);
    wA2MPreserveSpaces = new WrapperCheckBox(checkBox_A2M_preserve_spaces->text(), *checkBox_A2M_preserve_spaces, REQUIRED_FIELD);
    wA2MPreserveLinefeed = new WrapperCheckBox(checkBox_A2M_preserve_linefeed->text(), *checkBox_A2M_preserve_linefeed, REQUIRED_FIELD);
    wA2MDefaultParagraph = new WrapperQLineEdit(label_A2M_default_paragraph->text(), *lineEdit_A2M_default_paragraph, REQUIRED_FIELD);
    wA2MGraphWidth = new WrapperSpinBox(label_A2M_graph_width->text(), *spinBox_A2M_graph_width, REQUIRED_FIELD);
    wA2MGraphHeight = new WrapperSpinBox(label_A2M_graph_height->text(), *spinBox_A2M_graph_height, REQUIRED_FIELD);
    wA2MGraphBackgroundColor = new WrapperComboBox(label_A2M_graph_background_color->text(), *comboBox_A2M_graph_background_color, REQUIRED_FIELD, q_colors);
    wA2MGraphBackgroundBrush = new WrapperSpinBox(label_A2M_graph_background_brush->text(), *spinBox_A2M_graph_background_brush, REQUIRED_FIELD);
    wA2MGraphBox = new WrapperSpinBox(label_A2M_graph_box->text(), *spinBox_A2M_graph_box, REQUIRED_FIELD);

    comboBox_A2M_graph_background_color->setCurrentIndex(0);

    mapFields["A2M_Escape_Char"]            = wA2MEscapeChar;
    mapFields["A2M_Cell_Separator"]         = wA2MCellSeparator;
    mapFields["A2M_Define_Char"]            = wA2MDefineChar;
    mapFields["A2M_Command_Char"]           = wA2MCommandChar;
    mapFields["A2M_Append"]                 = wA2MAppend;
    mapFields["A2M_Preserve_Spaces"]        = wA2MPreserveSpaces;
    mapFields["A2M_Preserve_Linefeed"]      = wA2MPreserveLinefeed;
    mapFields["A2M_Default_Paragraph"]      = wA2MDefaultParagraph;
    mapFields["A2M_Graph_Width"]            = wA2MGraphWidth;
    mapFields["A2M_Graph_Height"]           = wA2MGraphHeight;
    mapFields["A2M_Graph_Background_Color"] = wA2MGraphBackgroundColor;
    mapFields["A2M_Graph_Background_Brush"] = wA2MGraphBackgroundBrush;
    mapFields["A2M_Graph_Box"]              = wA2MGraphBox;

    // RTF
    wRTFParagraphNumbers = new WrapperCheckBox(checkBox_RTF_paragraph_numbers->text(), *checkBox_RTF_paragraph_numbers, REQUIRED_FIELD);
    wRTFFontSize = new WrapperSpinBox(label_RTF_font_size->text(), *spinBox_RTF_font_size, REQUIRED_FIELD);
    wRTFFontIncr = new WrapperSpinBox(label_RTF_font_incr->text(), *spinBox_RTF_font_incr, REQUIRED_FIELD);
    wRTFFontFamily = new WrapperComboBox(label_RTF_font_family->text(), *comboBox_RTF_font_family, REQUIRED_FIELD, q_fonts);
    wRTFTableUseColor = new WrapperCheckBox(checkBox_RTF_table_use_color->text(), *checkBox_RTF_table_use_color, REQUIRED_FIELD);
    wRTFTableSplitTables = new WrapperCheckBox(checkBox_RTF_table_split_tables->text(), *checkBox_RTF_table_split_tables, REQUIRED_FIELD);
    wRTFTableOutsideBorders = new WrapperCheckBox(checkBox_RTF_table_outside_borders->text(), *checkBox_RTF_table_outside_borders, REQUIRED_FIELD);
    wRTFTableVerticalLines = new WrapperCheckBox(checkBox_RTF_table_vertical_lines->text(), *checkBox_RTF_table_vertical_lines, REQUIRED_FIELD);
    wRTFTableHorizontalLines = new WrapperCheckBox(checkBox_RTF_table_horizontal_lines->text(), *checkBox_RTF_table_horizontal_lines, REQUIRED_FIELD);
    wRTFTableWidth = new WrapperSpinBox(label_RTF_table_width->text(), *spinBox_RTF_table_width, REQUIRED_FIELD);
    wRTFTableFirstColWidth = new WrapperSpinBox(label_RTF_table_first_col_width->text(), *spinBox_RTF_table_first_col_width, REQUIRED_FIELD);
    wRTFTableOtherColWidth = new WrapperSpinBox(label_RTF_table_other_col_width->text(), *spinBox_RTF_table_other_col_width, REQUIRED_FIELD);
    wRTFTableFontSize = new WrapperSpinBox(label_RTF_table_font_size->text(), *spinBox_RTF_table_font_size, REQUIRED_FIELD);
    wRTFTableFontFamily = new WrapperComboBox(label_RTF_table_font_family->text(), *comboBox_RTF_table_font_family, REQUIRED_FIELD, q_fonts);
    wRTFHelpTitle = new WrapperQLineEdit(label_RTF_help_title->text(), *lineEdit_RTF_help_title, OPTIONAL_FIELD);
    wRTFCopyright = new WrapperQLineEdit(label_RTF_copyright->text(), *lineEdit_RTF_copyright, OPTIONAL_FIELD);
    wRTFPrepareForHCW = new WrapperCheckBox(checkBox_RTF_prepare_for_HCW->text(), *checkBox_RTF_prepare_for_HCW, REQUIRED_FIELD);
    wRTFCompressHelp = new WrapperCheckBox(checkBox_RTF_compress_help->text(), *checkBox_RTF_compress_help, REQUIRED_FIELD);
    wRTFGenerateTOC = new WrapperCheckBox(checkBox_RTF_generate_TOC->text(), *checkBox_RTF_generate_TOC, REQUIRED_FIELD);

    comboBox_RTF_font_family->setCurrentIndex(0);
    comboBox_RTF_table_font_family->setCurrentIndex(0);

    mapFields["RTF_Paragraph_Numbers"]      = wRTFParagraphNumbers;
    mapFields["RTF_Font_Size"]              = wRTFFontSize;
    mapFields["RTF_Font_Incr"]              = wRTFFontIncr;
    mapFields["RTF_Font_Family"]            = wRTFFontFamily;
    mapFields["RTF_Table_Use_Color"]        = wRTFTableUseColor;
    mapFields["RTF_Table_Split_Tables"]     = wRTFTableSplitTables;
    mapFields["RTF_Table_Outside_Borders"]  = wRTFTableOutsideBorders;
    mapFields["RTF_Table_Vertical_Lines"]   = wRTFTableVerticalLines;
    mapFields["RTF_Table_Horizontal_Lines"] = wRTFTableHorizontalLines;
    mapFields["RTF_Table_Width"]            = wRTFTableWidth;
    mapFields["RTF_Table_First_Col_Width"]  = wRTFTableFirstColWidth;
    mapFields["RTF_Table_Other_Col_Width"]  = wRTFTableOtherColWidth;
    mapFields["RTF_Table_Font_size"]        = wRTFTableFontSize;
    mapFields["RTF_Table_Font_Family"]      = wRTFTableFontFamily;
    mapFields["RTF_Help_Title"]             = wRTFHelpTitle;
    mapFields["RTF_Copyright"]              = wRTFCopyright;
    mapFields["RTF_Prepare_For_HCW"]        = wRTFPrepareForHCW;
    mapFields["RTF_Compress_Help"]          = wRTFCompressHelp;
    mapFields["RTF_Generate_TOC"]           = wRTFGenerateTOC;

    // HTML
    wHTMLFontSize = new WrapperSpinBox(label_HTML_font_size->text(), *spinBox_HTML_font_size, REQUIRED_FIELD);
    wHTMLFontIncr = new WrapperSpinBox(label_HTML_font_incr->text(), *spinBox_HTML_font_incr, REQUIRED_FIELD);
    wHTMLFontFamily = new WrapperComboBox(label_HTML_font_family->text(), *comboBox_HTML_font_family, REQUIRED_FIELD, q_fonts);
    wHTMLTableUseColor = new WrapperCheckBox(checkBox_HTML_table_use_color->text(), *checkBox_HTML_table_use_color, REQUIRED_FIELD);
    wHTMLTableBorderWidth = new WrapperSpinBox(label_HTML_table_border_width->text(), *spinBox_HTML_table_border_width, REQUIRED_FIELD);
    wHTMLTableFontSize = new WrapperSpinBox(label_HTML_table_font_size->text(), *spinBox_HTML_table_font_size, REQUIRED_FIELD);
    wHTMLTableFontFamily = new WrapperComboBox(label_HTML_table_font_family->text(), *comboBox_HTML_table_font_family, REQUIRED_FIELD, q_fonts);
    wHTMLParagraphNumbers = new WrapperCheckBox(checkBox_HTML_paragraph_numbers->text(), *checkBox_HTML_paragraph_numbers, REQUIRED_FIELD);
    wHTMLGenerateTOC = new WrapperCheckBox(checkBox_HTML_generate_TOC->text(), *checkBox_HTML_generate_TOC, REQUIRED_FIELD);
    wHTMLBodyTag = new WrapperQTextEdit(label_HTML_body_tag->text(), *textEdit_HTML_body_tag, OPTIONAL_FIELD);
    wHTMLTitle = new WrapperQLineEdit(label_HTML_title->text(), *lineEdit_HTML_title, OPTIONAL_FIELD);

    comboBox_HTML_font_family->setCurrentIndex(0);
    comboBox_HTML_table_font_family->setCurrentIndex(0);

    mapFields["HTML_Font_Size"]          = wHTMLFontSize;
    mapFields["HTML_Font_Incr"]          = wHTMLFontIncr;
    mapFields["HTML_Font_Family"]        = wHTMLFontFamily;
    mapFields["HTML_Table_Use_Color"]    = wHTMLTableUseColor;
    mapFields["HTML_Table_Border_Width"] = wHTMLTableBorderWidth;
    mapFields["HTML_Table_Font_Size"]    = wHTMLTableFontSize;
    mapFields["HTML_Table_Font_Family"]  = wHTMLTableFontFamily;
    mapFields["HTML_Paragraph_Numbers"]  = wHTMLParagraphNumbers;
    mapFields["HTML_Generate_TOC"]       = wHTMLGenerateTOC;
    mapFields["HTML_Body_Tag"]           = wHTMLBodyTag;
    mapFields["HTML_Title"]              = wHTMLTitle;

    // MIF
    wMIFFontSize = new WrapperSpinBox(label_MIF_font_size->text(), *spinBox_MIF_font_size, REQUIRED_FIELD);
    wMIFFontIncr = new WrapperSpinBox(label_MIF_font_incr->text(), *spinBox_MIF_font_incr, REQUIRED_FIELD);
    wMIFFontFamily = new WrapperComboBox(label_MIF_font_family->text(), *comboBox_MIF_font_family, REQUIRED_FIELD, q_fonts);
    wMIFTableFontFamily = new WrapperComboBox(label_MIF_table_font_family->text(), *comboBox_MIF_table_font_family, REQUIRED_FIELD, q_fonts);
    wMIFTableFontSize = new WrapperSpinBox(label_MIF_table_font_size->text(), *spinBox_MIF_table_font_size, REQUIRED_FIELD);
    wMIFTableUseColor = new WrapperCheckBox(checkBox_MIF_table_use_color->text(), *checkBox_MIF_table_use_color, REQUIRED_FIELD);
    wMIFTableFirstColWidth = new WrapperSpinBox(label_MIF_table_first_col_width->text(), *spinBox_MIF_table_first_col_width, REQUIRED_FIELD);
    wMIFTableOtherColWidth = new WrapperSpinBox(label_MIF_table_other_col_width->text(), *spinBox_MIF_table_other_col_width, REQUIRED_FIELD);
    wMIFTableSplit = new WrapperCheckBox(checkBox_MIF_table_split_tables->text(), *checkBox_MIF_table_split_tables, REQUIRED_FIELD);
    wMIFTableWidth = new WrapperSpinBox(label_MIF_table_width->text(), *spinBox_MIF_table_width, REQUIRED_FIELD);
    wMIFTableOutsideBorders = new WrapperCheckBox(checkBox_MIF_table_outside_borders->text(), *checkBox_MIF_table_outside_borders, REQUIRED_FIELD);
    wMIFTableHorizontalLines = new WrapperCheckBox(checkBox_MIF_table_horizontal_lines->text(), *checkBox_MIF_table_horizontal_lines, REQUIRED_FIELD);
    wMIFTableVericalLines = new WrapperCheckBox(checkBox_MIF_table_vertical_lines->text(), *checkBox_MIF_table_vertical_lines, REQUIRED_FIELD);
    wMIFImageRefInText = new WrapperCheckBox(checkBox_MIF_image_ref_in_text->text(), *checkBox_MIF_image_ref_in_text, REQUIRED_FIELD);

    comboBox_MIF_font_family->setCurrentIndex(0);
    comboBox_MIF_table_font_family->setCurrentIndex(0);

    mapFields["MIF_Font_Size"]              = wMIFFontSize;
    mapFields["MIF_Incr"]                   = wMIFFontIncr;
    mapFields["MIF_Font_Family"]            = wMIFFontFamily;
    mapFields["MIF_Table_Font_Family"]      = wMIFTableFontFamily;
    mapFields["MIF_Table_Font_Size"]        = wMIFTableFontSize;
    mapFields["MIF_Table_Use_Color"]        = wMIFTableUseColor;
    mapFields["MIF_Table_First_Col_Width"]  = wMIFTableFirstColWidth;
    mapFields["MIF_Table_Other_Col_Width"]  = wMIFTableOtherColWidth;
    mapFields["MIF_Table_Split"]            = wMIFTableSplit;
    mapFields["MIF_Table_Width"]            = wMIFTableWidth;
    mapFields["MIF_Table_Outside_Borders"]  = wMIFTableOutsideBorders;
    mapFields["MIF_Table_Horizontal_Lines"] = wMIFTableHorizontalLines;
    mapFields["MIF_Table_Verical_Lines"]    = wMIFTableVericalLines;
    mapFields["MIF_Image_Ref_In_Text"]      = wMIFImageRefInText;

    className = "MENU_FILE_PRINT_PREFERENCES";
    loadSettings();
}

FilePrintPreferences::~FilePrintPreferences()
{
    // PRINTER
    delete wPrinterFontSize;
    delete wPrinterFontFamily;
    delete wPrinterTableFontFamily;
    delete wPrinterFontIncr;
    delete wPrinterTableFontSize;
    delete wPrinterParagraphNumbers;
    delete wPrinterLeftMargin;
    delete wPrinterRightMargin;
    delete wPrinterTopMargin;
    delete wPrinterBottomMargin;
    delete wPrinterTableBorderWidth;
    delete wPrinterBlackAndWhite;
    delete wPrinterTableBreak;
    delete wPrinterTableNewPage;
    delete wPrinterGraphNewPage;
    delete wPrinterTableShading;
    delete wPrinterPageHeader;
    delete wPrinterPageFooter;
    delete wPrinterPrompt;

    // A2M
    delete wA2MEscapeChar;
    delete wA2MCellSeparator;
    delete wA2MDefineChar;
    delete wA2MCommandChar;
    delete wA2MAppend;
    delete wA2MPreserveSpaces;
    delete wA2MPreserveLinefeed;
    delete wA2MDefaultParagraph;
    delete wA2MGraphWidth;
    delete wA2MGraphHeight;
    delete wA2MGraphBackgroundColor;
    delete wA2MGraphBackgroundBrush;
    delete wA2MGraphBox;

    // RTF
    delete wRTFParagraphNumbers;
    delete wRTFFontSize;
    delete wRTFFontIncr;
    delete wRTFFontFamily;
    delete wRTFTableUseColor;
    delete wRTFTableSplitTables;
    delete wRTFTableOutsideBorders;
    delete wRTFTableVerticalLines;
    delete wRTFTableHorizontalLines;
    delete wRTFTableWidth;
    delete wRTFTableFirstColWidth;
    delete wRTFTableOtherColWidth;
    delete wRTFTableFontSize;
    delete wRTFTableFontFamily;
    delete wRTFHelpTitle;
    delete wRTFCopyright;
    delete wRTFPrepareForHCW;
    delete wRTFCompressHelp;
    delete wRTFGenerateTOC;

    // HTML
    delete wHTMLFontSize;
    delete wHTMLFontIncr;
    delete wHTMLFontFamily;
    delete wHTMLTableUseColor;
    delete wHTMLTableBorderWidth;
    delete wHTMLTableFontSize;
    delete wHTMLTableFontFamily;
    delete wHTMLParagraphNumbers;
    delete wHTMLGenerateTOC;
    delete wHTMLBodyTag;
    delete wHTMLTitle;

    // MIF
    delete wMIFFontSize;
    delete wMIFFontIncr;
    delete wMIFFontFamily;
    delete wMIFTableFontFamily;
    delete wMIFTableFontSize;
    delete wMIFTableUseColor;
    delete wMIFTableFirstColWidth;
    delete wMIFTableOtherColWidth;
    delete wMIFTableSplit;
    delete wMIFTableWidth;
    delete wMIFTableOutsideBorders;
    delete wMIFTableHorizontalLines;
    delete wMIFTableVericalLines;
    delete wMIFImageRefInText;
}

/**
 * @brief 
 * 
 * @note see B_A2mGetGdiParms() from DOS_SB_PDEST.C (old GUI)
 */
void FilePrintPreferences::set_printer_preferences()
{
    extern  int W_gdiask; /* GB 23/03/98 */
    extern  int W_gdiduplex, W_gdiorient; /* JMP 18-04-98 */

    SCR_free(A2M_PGHEAD);
    SCR_free(A2M_PGFOOT);

    A2M_FONTSIZE    = wPrinterFontSize->extractAndVerify();
    A2M_FONTFAMILY  = "HTCBP"[wPrinterFontFamily->extractAndVerify()];
    A2M_TFONTFAMILY = "HTCBP"[wPrinterTableFontFamily->extractAndVerify()];
    A2M_FONTINCR    = wPrinterFontIncr->extractAndVerify();
    A2M_TFONTSIZE   = wPrinterTableFontSize->extractAndVerify();
    A2M_NUMBERS     = wPrinterParagraphNumbers->extractAndVerify();
#ifndef UNIX
    A2M_GDI_LMARG   = wPrinterLeftMargin->extractAndVerify();
    A2M_GDI_RMARG   = wPrinterRightMargin->extractAndVerify();
    A2M_GDI_TMARG   = wPrinterTopMargin->extractAndVerify();
    A2M_GDI_BMARG   = wPrinterBottomMargin->extractAndVerify();
    A2M_GDI_COLOR   = !wPrinterBlackAndWhite->extractAndVerify();
    A2M_GDI_TBORDER = wPrinterTableBorderWidth->extractAndVerify();
#endif
    A2M_TBREAK      = wPrinterTableBreak->extractAndVerify();
    A2M_TPAGE       = wPrinterTableNewPage->extractAndVerify();
    A2M_GPAGE       = wPrinterGraphNewPage->extractAndVerify();
    if(wPrinterTableShading->extractAndVerify()) 
    {
	    A2M_TSHADING_BRUSH[0] = 7;
	    A2M_TSHADING_BRUSH[1] = 5;
	}
    else 
    {
	    A2M_TSHADING_BRUSH[0] = 0;
	    A2M_TSHADING_BRUSH[1] = 0;
	}
    A2M_PGHEAD = SCR_stracpy((unsigned char *) wPrinterPageHeader->extractAndVerify().toStdString().data());
    A2M_PGFOOT = SCR_stracpy((unsigned char *) wPrinterPageFooter->extractAndVerify().toStdString().data());

    W_gdiask = wPrinterPrompt->extractAndVerify();
    //W_gdiduplex = wPrinter_DUPLEX;    // JMP 22/8/2016
    //W_gdiorient = wPrinter_ORIENT;    // JMP 22/8/2016

    SCR_strip(A2M_PGHEAD);
    SCR_strip(A2M_PGFOOT);
}

/**
 * @brief 
 * 
 * @note see B_A2mGetGnlParms() from DOS_SB_PDEST.C (old GUI)
 */
void FilePrintPreferences::set_A2M_preferences()
{
    extern int W_a2mapp;

    // control characters
    A2M_ESCCH = wA2MEscapeChar->extractAndVerify()[0].toLatin1();
    A2M_SEPCH = wA2MCellSeparator->extractAndVerify()[0].toLatin1();
    A2M_DEFCH = wA2MDefineChar->extractAndVerify()[0].toLatin1();
    A2M_CMDCH = wA2MCommandChar->extractAndVerify()[0].toLatin1();

    // Startup Options
    W_a2mapp  = wA2MAppend->extractAndVerify();
    A2M_BLON  = wA2MPreserveSpaces->extractAndVerify();
    A2M_LFON  = wA2MPreserveLinefeed->extractAndVerify();
    char* tag = wA2MDefaultParagraph->extractAndVerify().toStdString().data();
    strcpy((char *) A2M_CURTAG, tag);
    SCR_sqz(A2M_CURTAG);

    // Graphs
    A2M_GWIDTH  = 0.1 * wA2MGraphWidth->extractAndVerify();
    A2M_GHEIGHT = 0.1 * wA2MGraphHeight->extractAndVerify();
    A2M_BACKCOLOR = "byBwrgcm"[wA2MGraphBackgroundColor->extractAndVerify()];
    A2M_BACKBRUSH = wA2MGraphBackgroundBrush->extractAndVerify();
    A2M_BOXWIDTH  = wA2MGraphBox->extractAndVerify();
}

/**
 * @brief 
 * 
 * @note see B_A2mGetMifParms() from DOS_SB_PDEST.C (old GUI)
 * 
 */
void FilePrintPreferences::set_MIF_preferences()
{
    A2M_FONTSIZE     = wMIFFontSize->extractAndVerify();
    A2M_FONTINCR     = wMIFFontIncr->extractAndVerify();
    A2M_FONTFAMILY   = "HTCBP"[wMIFFontFamily->extractAndVerify()];
    A2M_TFONTFAMILY  = "HTCBP"[wMIFTableFontFamily->extractAndVerify()];
    A2M_TFONTSIZE    = wMIFTableFontSize->extractAndVerify();
    A2M_FRM_TCOLOR   = wMIFTableUseColor->extractAndVerify();
    A2M_FRM_TCOL1    = wMIFTableFirstColWidth->extractAndVerify();
    A2M_FRM_TCOLN    = wMIFTableOtherColWidth->extractAndVerify();
    A2M_FRM_TSPLIT   = wMIFTableSplit->extractAndVerify();
    A2M_FRM_TWIDTH   = wMIFTableWidth->extractAndVerify();
    A2M_FRM_TBORDER  = wMIFTableOutsideBorders->extractAndVerify();
    A2M_FRM_HLINES   = wMIFTableHorizontalLines->extractAndVerify();
    A2M_FRM_VLINES   = wMIFTableVericalLines->extractAndVerify();
    A2M_FRM_REF      = wMIFImageRefInText->extractAndVerify();
}

/**
 * @brief 
 * 
 * @note see B_A2mGetRtfParms() from DOS_SB_PDEST.C (old GUI)
 * 
 */
void FilePrintPreferences::set_RTF_preferences()
{
    A2M_FONTSIZE     = wRTFFontSize->extractAndVerify();
    A2M_FONTFAMILY  = "HTCBP"[wRTFFontFamily->extractAndVerify()];
    A2M_TFONTFAMILY = "HTCBP"[wRTFTableFontFamily->extractAndVerify()];
    A2M_FONTINCR     = wRTFFontIncr->extractAndVerify();
    A2M_TFONTSIZE    = wRTFTableFontSize->extractAndVerify();
    A2M_RTF_TCOLOR   = wRTFTableUseColor->extractAndVerify();
    A2M_RTF_TCOL1    = wRTFTableFirstColWidth->extractAndVerify();
    A2M_RTF_TCOLN    = wRTFTableOtherColWidth->extractAndVerify();
    A2M_RTF_TSPLIT   = wRTFTableSplitTables->extractAndVerify();
    A2M_RTF_TWIDTH   = wRTFTableWidth->extractAndVerify();
    A2M_RTF_TBORDER  = wRTFTableOutsideBorders->extractAndVerify();
    A2M_RTF_HLINES   = wRTFTableHorizontalLines->extractAndVerify();
    A2M_RTF_VLINES   = wRTFTableVerticalLines->extractAndVerify();
    W_rtfhelp        = wRTFPrepareForHCW->extractAndVerify();
    A2M_RTF_COMPRESS = wRTFCompressHelp->extractAndVerify();
    A2M_NUMBERS      = wRTFParagraphNumbers->extractAndVerify();
    A2M_RTF_TOC      = wRTFGenerateTOC->extractAndVerify();

    B_A2mSetRtfTitle((unsigned char *) wRTFHelpTitle->extractAndVerify().toStdString().data());
    B_A2mSetRtfCopy((unsigned char *) wRTFCopyright->extractAndVerify().toStdString().data());
}

/**
 * @brief 
 * 
 * @note see B_A2mGetHtmlParms() from DOS_SB_PDEST.C (old GUI) 
 * 
 */
void FilePrintPreferences::set_HTML_preferences()
{
    SCR_free(A2M_HTML_BODY);
    SCR_free(A2M_HTML_TITLE);

    A2M_FONTSIZE     = wHTMLFontSize->extractAndVerify();
    A2M_FONTINCR     = wHTMLFontIncr->extractAndVerify();
    A2M_FONTFAMILY   = "HTCBP"[wHTMLFontFamily->extractAndVerify()];
    A2M_TFONTFAMILY  = "HTCBP"[wHTMLTableFontFamily->extractAndVerify()];
    A2M_TFONTSIZE    = wHTMLTableFontSize->extractAndVerify();
    A2M_NUMBERS      = wHTMLParagraphNumbers->extractAndVerify();
    A2M_HTML_TBORDER = wHTMLTableBorderWidth->extractAndVerify();
    A2M_HTML_TCOLOR  = wHTMLTableUseColor->extractAndVerify();
    A2M_HTML_TOC     = wHTMLGenerateTOC->extractAndVerify();
    A2M_HTML_BODY    = (char *) SCR_stracpy((unsigned char *) wHTMLBodyTag->extractAndVerify().toStdString().data());
    A2M_HTML_TITLE   = SCR_stracpy((unsigned char *) wHTMLTitle->extractAndVerify().toStdString().data());

    SCR_strip((unsigned char *) A2M_HTML_BODY);
    SCR_strip((unsigned char *) A2M_HTML_TITLE);
}

void FilePrintPreferences::set_print_preferences()
{
    try
    {
        set_printer_preferences();
        set_A2M_preferences();
        set_MIF_preferences();
        set_RTF_preferences();
        set_HTML_preferences();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("WARNING"), tr(e.what()));
    }

    this->accept();
}

void FilePrintPreferences::help()
{
	QDesktopServices::openUrl(url_manual);
}
