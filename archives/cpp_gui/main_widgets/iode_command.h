#pragma once
#include "main_window_abstract.h"
#include "text_edit/complete_line_edit.h"
#include "settings.h"

#include <QTextEdit>
#include <QStringList>


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
class IodeCommandLine: public IodeAutoCompleteLineEdit
{
    Q_OBJECT

    const static int MAX_NB_COMMANDS_TO_REMEMBER = 20;
    const static inline QString SETTINGS_GROUP_NAME = "IODE_COMMAND_LINE";

    QStringList executedCommandsList;
    QStringList::const_iterator it;            // const_iterator are for read-only access

    QTextEdit* output; 

public:
    IodeCommandLine(QWidget *parent = nullptr) 
        : IodeAutoCompleteLineEdit(parent), it(executedCommandsList.end()) {}

    ~IodeCommandLine() 
    {
        saveSettings();
    }

    void setup() 
    { 
        MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
        this->output = main_window->getOutput();
        setCompleter(main_window->getCompleter());
        loadSettings();
    }

protected:
    /**
     * @param event 
     * @return bool weither or not to exit keyPressEvent() 
     */
    bool handleSpecialKeys(QKeyEvent *event);

    /**
     * @brief store the last MAX_NB_COMMANDS_TO_REMEMBER executed commands to the project settings file
     * 
     */
    void saveSettings();

    /**
     * @brief load the list of the last MAX_NB_COMMANDS_TO_REMEMBER commands executed in the previous GUI session. 
     * 
     */
    void loadSettings();

public slots:
    void run_command();

signals:
    void askComputeHash(bool);
};
