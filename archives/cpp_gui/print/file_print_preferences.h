#pragma once

#include <QWidget>
#include <QList>

#include "ui_file_print_preferences.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


class FilePrintPreferences : public IodeSettingsDialog, public Ui::FilePrintPreferences
{
    Q_OBJECT

    // PRINTER tab
    WrapperSpinBox*   wPrinterFontSize;
    WrapperComboBox*  wPrinterFontFamily;
    WrapperComboBox*  wPrinterTableFontFamily;
    WrapperSpinBox*   wPrinterFontIncr;
    WrapperSpinBox*   wPrinterTableFontSize;
    WrapperCheckBox*  wPrinterParagraphNumbers;
    WrapperSpinBox*   wPrinterLeftMargin;
    WrapperSpinBox*   wPrinterRightMargin;
    WrapperSpinBox*   wPrinterTopMargin;
    WrapperSpinBox*   wPrinterBottomMargin;
    WrapperSpinBox*   wPrinterTableBorderWidth;
    WrapperCheckBox*  wPrinterBlackAndWhite;
    WrapperCheckBox*  wPrinterTableBreak;
    WrapperCheckBox*  wPrinterTableNewPage;
    WrapperCheckBox*  wPrinterGraphNewPage;
    WrapperCheckBox*  wPrinterTableShading;
    WrapperQLineEdit* wPrinterPageHeader;
    WrapperQLineEdit* wPrinterPageFooter;
    WrapperCheckBox*  wPrinterPrompt;

    // A2M tab
    WrapperQLineEdit* wA2MEscapeChar;
    WrapperQLineEdit* wA2MCellSeparator;
    WrapperQLineEdit* wA2MDefineChar;
    WrapperQLineEdit* wA2MCommandChar;
    WrapperCheckBox*  wA2MAppend;
    WrapperCheckBox*  wA2MPreserveSpaces;
    WrapperCheckBox*  wA2MPreserveLinefeed;
    WrapperQLineEdit* wA2MDefaultParagraph;
    WrapperSpinBox*   wA2MGraphWidth;
    WrapperSpinBox*   wA2MGraphHeight;
    WrapperComboBox*  wA2MGraphBackgroundColor;
    WrapperSpinBox*   wA2MGraphBackgroundBrush;
    WrapperSpinBox*   wA2MGraphBox;

    // RTF tab
    WrapperCheckBox*  wRTFParagraphNumbers;
    WrapperSpinBox*   wRTFFontSize;
    WrapperSpinBox*   wRTFFontIncr;
    WrapperComboBox*  wRTFFontFamily;
    WrapperCheckBox*  wRTFTableUseColor;
    WrapperCheckBox*  wRTFTableSplitTables;
    WrapperCheckBox*  wRTFTableOutsideBorders;
    WrapperCheckBox*  wRTFTableVerticalLines;
    WrapperCheckBox*  wRTFTableHorizontalLines;
    WrapperSpinBox*   wRTFTableWidth;
    WrapperSpinBox*   wRTFTableFirstColWidth;
    WrapperSpinBox*   wRTFTableOtherColWidth;
    WrapperSpinBox*   wRTFTableFontSize;
    WrapperComboBox*  wRTFTableFontFamily;
    WrapperQLineEdit* wRTFHelpTitle;
    WrapperQLineEdit* wRTFCopyright;
    WrapperCheckBox*  wRTFPrepareForHCW;
    WrapperCheckBox*  wRTFCompressHelp;
    WrapperCheckBox*  wRTFGenerateTOC;

    // HTML tab
    WrapperSpinBox*   wHTMLFontSize;
    WrapperSpinBox*   wHTMLFontIncr;
    WrapperComboBox*  wHTMLFontFamily;
    WrapperCheckBox*  wHTMLTableUseColor;
    WrapperSpinBox*   wHTMLTableBorderWidth;
    WrapperSpinBox*   wHTMLTableFontSize;
    WrapperComboBox*  wHTMLTableFontFamily;
    WrapperCheckBox*  wHTMLParagraphNumbers;
    WrapperCheckBox*  wHTMLGenerateTOC;
    WrapperQTextEdit* wHTMLBodyTag;
    WrapperQLineEdit* wHTMLTitle;

    // MIF tab
    WrapperSpinBox*   wMIFFontSize;
    WrapperSpinBox*   wMIFFontIncr;
    WrapperComboBox*  wMIFFontFamily;
    WrapperComboBox*  wMIFTableFontFamily;
    WrapperSpinBox*   wMIFTableFontSize;
    WrapperCheckBox*  wMIFTableUseColor;
    WrapperSpinBox*   wMIFTableFirstColWidth;
    WrapperSpinBox*   wMIFTableOtherColWidth;
    WrapperCheckBox*  wMIFTableSplit;
    WrapperSpinBox*   wMIFTableWidth;
    WrapperCheckBox*  wMIFTableOutsideBorders;
    WrapperCheckBox*  wMIFTableHorizontalLines;
    WrapperCheckBox*  wMIFTableVericalLines;
    WrapperCheckBox*  wMIFImageRefInText;

public:
	FilePrintPreferences(QWidget* parent = nullptr);
	~FilePrintPreferences();

private:
    void set_printer_preferences();
    void set_A2M_preferences();
    void set_MIF_preferences();
    void set_RTF_preferences();
    void set_HTML_preferences();

public slots:
    void set_print_preferences();
    void help();
};
