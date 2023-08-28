#pragma once
#include "main_window_abstract.h"
#include "text_edit/complete_line_edit.h"

#include <QTextEdit>
#include <QVector>


/**
 * @brief Special line field to execute an Iode command.
 *        It works like a 1-line terminal.
 * 
 *        The command is executed when the user presses Enter.
 *        If the command succeeded:
 *        - the command is added to the list of executed commands
 *        - the text "> <command>" is printed in the output panel
 *        - the content of the command field is erased
 *        If the command failed:
 *        - a warning box is displayed with error message
 *        - the error message is copied in the output panel
 * 
*         Pressing up/down allows to reuse a previously executed command
 *        (like in a terminal).
 * 
 *        Auto-completion starts at the beginning of the line 
 *        or after a whitespace or any characters which is not 
 *        A-Z, a-z, 0-9, _, $, #, and @. 
 * 
 */
class QIodeCommandLine: public QIodeCompleteLineEdit
{
    Q_OBJECT

    QVector<QString> executedCommandsList;
    QVector<QString>::const_iterator it;            // const_iterator are for read-only access

    QTextEdit* output; 

public:
    QIodeCommandLine(QWidget *parent = nullptr) 
        : QIodeCompleteLineEdit(parent), it(executedCommandsList.end()) {}

    void setup() 
    { 
        MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
        this->output = main_window->getOutput();
        setCompleter(main_window->getCompleter());
    }

protected:
    /**
     * @param event 
     * @return bool weither or not to exit keyPressEvent() 
     */
    bool handleSpecialKeys(QKeyEvent *event);

public slots:
    void run_command();

signals:
    void askComputeHash(bool);
};
