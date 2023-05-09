#pragma once

#include <QWidget>
#include <QString>
#include <QPrinter>
#include <QStringList>
#include <QTextDocument>
#include <QPrintPreviewDialog>

#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "menu/file/file_print_setup.h"
#include "custom_widgets/print_file_dialog.h"

/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class QIodeMenuPrintAbstract: public QIodeSettings
{
    Q_OBJECT 

    QPrinter printer;
    QTextDocument document;
    const static QString TMP_FILENAME;

protected:
    WrapperQTextEdit*        wSample;
    WrapperFileChooser*      wFile2;
    WrapperFileChooser*      wFile3;
    WrapperFileChooser*      wFile4;
    WrapperFileChooser*      wFile5;
    WrapperComboBox*         wLanguage;
    WrapperSpinBox*          wNbDecimals;

public:
    QIodeMenuPrintAbstract(const QString& settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) 
        : QIodeSettings(settings_filepath, parent, f) {}

    ~QIodeMenuPrintAbstract() {}

protected:

    /**
     * @brief 
     * 
     * @note see C_ViewPrintGrTblVar() in sb_view.c from the old GUI
     */
    void printTableOrVariable(const bool isTable, const QString& names);

    /**
     * @brief Set the Print Dest object
     * 
     * @note see C_PrintDestDecLang() in sb_pdest.c from the old GUI
     * 
     * @param outputFile 
     * @param format 
     */
    void setPrintDest(const QString& outputFile, const QChar& format=QChar::Null);

    /**
     * @brief Set the Nb Decimals object
     * 
     * @note see C_PrintDestDecLang() in sb_pdest.c from the old GUI
     * 
     * @param nbDecimals 
     */
    void setNbDecimals(const int nbDecimals);

    /**
     * @brief Set the Lang object
     * 
     * @note see C_PrintDestDecLang() in sb_pdest.c from the old GUI
     * 
     * @param lang 
     */
    void setLang(const EnumLang lang);
    
    /**
     * @brief Return the string "<sample> <tables names>"
     * 
     * @note see C_ViewTblArgs() in sb_view.c from the old GUI
     * 
     * @param names
     * @param sample  
     */
    QString setTablesOrVariablesArgs(const QString& names, const QString& gsample);

    /**
     * @brief Set the Tables Files object.
     *        Return the string "<file number 2> <filepath 2> ... <file number 5> <filepath 5>"
     * 
     * @note see C_ViewTblFiles() in sb_view.c from the old GUI
     * 
     * @param files 
     */
    void setTablesFiles(const QStringList& files);

private slots:
    void renderForPrinting()
    {
        document.print(&printer);
    }
};
