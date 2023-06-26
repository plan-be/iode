#pragma once

#include <QShortcut>
#include <QScrollBar>
#include <QTextCursor>
#include <QAbstractItemView>

#include "utils.h"
#include "highlighter.h"
#include "completer.h"
#include "text_editor.h"
#include "util/print.h"


/**
 * @brief Widget allowing to edit an Iode report.
 * 
 *        CTRL+Space: forces auto-completion
 *        Tab | Return | Space: stops auto-completion
 * 
 * @note For syntax highlight, see example https://doc.qt.io/qt-6/qtwidgets-richtext-syntaxhighlighter-example.html 
 *       For auto-completion, see example https://doc.qt.io/qt-6/qtwidgets-tools-customcompleter-example.html 
 */
class ReportEditor : public TextEditor
{
    Q_OBJECT

    QIodeHighlighter *highlighter;
    QTextEdit* output;

public: 
    ReportEditor(QWidget *parent = nullptr);
    ~ReportEditor();

    void setOutput(QTextEdit* output)
    {
        this->output = output;
    }

    /**
     * @brief executes an IODE report.
     * 
     * Note: See function C_ReportExec() from the file sb_rep.c from the old GUI. 
     * 
     * @param filepath 
     * @param parameters 
     * @param nbDecimals
     * @param language
     */
    void run(const QString& filepath, const QString& parameters, const int nbDecimals, const EnumLang language);
};
