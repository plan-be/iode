#pragma once

#include <QString>
#include <QPrinter>
#include <QStringList>
#include <QTextDocument>
#include <QPrintPreviewDialog>

#include "utils.h"

const static QString TMP_FILENAME = "tmp.h";


/**
 * @brief 
 * 
 * @note see C_ViewPrintGrTblVar() in sb_view.c from the old GUI
 */
void printTableToFile(const QString& outputFile, const QChar& format, const QString& names, 
    const QString& gsample, const QStringList& files, const int nbDecimals, const EnumLang language);

/**
 * @brief 
 * 
 * @note see C_ViewPrintGrTblVar() in sb_view.c from the old GUI
 */
void printTable(QPrintPreviewDialog& dialog, QTextDocument& document, 
    const QString& names, const QString& gsample, const QStringList& files, 
    const int nbDecimals, const EnumLang language);

/**
 * @brief 
 * 
 * @note see C_ViewPrintGrTblVar() in sb_view.c from the old GUI
 */
void printVariableToFile(const QString& outputFile, const QChar& format, const QString& names, 
    const QString& gsample, const QStringList& files, const int nbDecimals, const EnumLang language);

/**
 * @brief 
 * 
 * @note see C_ViewPrintGrTblVar() in sb_view.c from the old GUI
 */
void printVariable(QPrintPreviewDialog& dialog, QTextDocument& document, 
    const QString& names, const QString& gsample, const QStringList& files, 
    const int nbDecimals, const EnumLang language);

/**
 * @brief Set the Print Dest object
 * 
 * @note see C_PrintDestDecLang() in sb_pdest.c from the old GUI
 * 
 * @param outputFile 
 * @param format 
 */
inline void setPrintDest(const QString& outputFile, const QChar& format=QChar::Null)
{
    // set output file
    // output_file = "<filepath> <format>"
    // if windows printer -> output_file = ""
    // see C_FilePrintFile() from sb_file.c from the old GUI
    std::string output_file = "";

    if(outputFile.isEmpty() || format.isNull())
        return;
    
    output_file = outputFile.toStdString() + " " + format.toLatin1();
    int res = B_PrintDest(output_file.data());
    if(res < 0)
        B_display_last_error();
}

/**
 * @brief Set the Nb Decimals object
 * 
 * @note see C_PrintDestDecLang() in sb_pdest.c from the old GUI
 * 
 * @param nbDecimals 
 */
inline void setNbDecimals(const int nbDecimals)
{
    // set number of decimals to print
    std::string nb_decimals = QString::number(nbDecimals).toStdString();
    int res = B_PrintNbDec(nb_decimals.data());
    if(res < 0)
        B_display_last_error();
}

/**
 * @brief Set the Lang object
 * 
 * @note see C_PrintDestDecLang() in sb_pdest.c from the old GUI
 * 
 * @param lang 
 */
inline void setLang(const EnumLang lang)
{
    if(lang > 2)
        throw IodeException("Invalid language.\nExpected value 0 (English), 1 (FRENCH), 2 (DUTCH)");

    // set lang
    char tlang[2];
    tlang[0] = "EFD"[lang];
    tlang[1] = 0;
    
    int res = B_PrintLang(tlang);
    if(res < 0)
        B_display_last_error();
}

/**
 * @brief Set the Tables Files object.
 *        Return the string "<file number 2> <filepath 2> ... <file number 5> <filepath 5>"
 * 
 * @note see C_ViewTblFiles() in sb_view.c from the old GUI
 * 
 * @param files 
 */
inline void setTablesFiles(const QStringList& files)
{
    int res;
    QString args;
    int i=2;
    foreach(const QString file, files)
    {
        args = QString::number(i) + " " + file;
        res = B_ViewTblFile(args.toStdString().data());
        if(res < 0)
        {
            IodeExceptionFunction error("Function B_ViewTblFile returned an error", "Unknown");
            error.add_argument("file number", QString::number(i).toStdString());
            error.add_argument("filepath", file.toStdString());
            throw error;
        }
        i++;
    }
}