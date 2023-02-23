#pragma once

#include <QShortcut>
#include <QScrollBar>
#include <QTextCursor>
#include <QAbstractItemView>

#include "utils.h"
#include "qiode_highlighter.h"
#include "qiode_completer.h"
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

    std::shared_ptr<QIodeCompleter> c;
    QIodeHighlighter *highlighter;

    QTextEdit* output; 

private:
    QString textUnderCursor() const;

private slots:
    void insertCompletion(const QString& completion);

protected:
    void keyPressEvent(QKeyEvent* e) override;

    // event handler used to receive keyboard focus events for the widget.
    void focusInEvent(QFocusEvent* e) override;

public: 
    ReportEditor(std::shared_ptr<QIodeCompleter>& c, QTextEdit* output, QWidget *parent = nullptr);
    ~ReportEditor();

    void run(const QString& filepath);
};
