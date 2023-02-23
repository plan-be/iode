#pragma once

#include "utils.h"
#include "custom_widgets/qiode_completer.h"

#include <QObject>
#include <QShortcut>
#include <QLineEdit>
#include <QTextEdit>
#include <QCompleter>
#include <QString>
#include <QVector>
#include <QKeyEvent>
#include <QRegularExpression>
#include <QAbstractItemView>


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
class QIodeCommandLine: public QLineEdit
{
    Q_OBJECT

    QVector<QString> executedCommandsList;
    QVector<QString>::const_iterator it;            // const_iterator are for read-only access

    std::shared_ptr<QIodeCompleter> c;

    QTextEdit* output; 

public:
    QIodeCommandLine(QWidget *parent = nullptr) : QLineEdit(parent), 
        it(executedCommandsList.end()), c(nullptr) {}

    void setup(QTextEdit* output, std::shared_ptr<QIodeCompleter>& c) 
    { 
        this->output = output;

        this->c = c;

        if (!c.get())
            return;

        c->setWidget(this);
        // Signal activated is overloaded in QCompleter. 
        // To connect to this signal by using the function pointer syntax, Qt provides 
        // a convenient helper for obtaining the function pointer
        connect(c.get(), QOverload<const QString &>::of(&QCompleter::activated),
                this, &QIodeCommandLine::insertCompletion);
    }

protected:
    void keyPressEvent(QKeyEvent *event) override;

    // event handler used to receive keyboard focus events for the widget.
    void focusInEvent(QFocusEvent* event) override;

private slots:
    void insertCompletion(const QString& completion);

private:
    QString textUnderCursor();

public slots:
    void run_command();
};
