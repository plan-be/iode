#pragma once

#include <QShortcut>
#include <QScrollBar>
#include <QTextCursor>
#include <QAbstractItemView>

#include "utils.h"
#include "settings.h"
#include "highlighter.h"
#include "completer.h"
#include "text_editor.h"
#include "print/print.h"
#include "menu/file/file_settings.h"
#include "main_window_abstract.h"


/**
 * @brief Widget allowing to edit an Iode report.
 * 
 *        CTRL+Space: forces auto-completion
 *        Tab | Return | Space: stops auto-completion
 * 
 * @note For syntax highlight, see example https://doc.qt.io/qt-6/qtwidgets-richtext-syntaxhighlighter-example.html 
 *       For auto-completion, see example https://doc.qt.io/qt-6/qtwidgets-tools-customcompleter-example.html 
 */
class IodeReportEditor : public IodeTextEditor
{
    Q_OBJECT

    IodeHighlighter *highlighter;
    QTextEdit* output;

public: 
    IodeReportEditor(QWidget *parent = nullptr);
    ~IodeReportEditor();

    void setup()
    {
        MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
        this->output = main_window->getOutput();
        setCompleter(main_window->getCompleter());
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
    void run(const QString& filepath, const QString& parameters, const int nbDecimals, const TableLang language);
};
