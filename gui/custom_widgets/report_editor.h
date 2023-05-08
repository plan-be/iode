#pragma once

#include <QShortcut>
#include <QScrollBar>
#include <QTextCursor>
#include <QAbstractItemView>

#include "utils.h"
#include "highlighter.h"
#include "completer.h"
#include "text_editor.h"


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
    ReportEditor(QTextEdit* output, QWidget *parent = nullptr);
    ~ReportEditor();

    void run(const QString& filepath);
};
