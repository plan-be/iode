#pragma once

#include "utils.h"
#include "custom_widgets/qiode_completer.h"

#include <QObject>
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

    QRegularExpression re;
    QIodeCompleter* c;
    QTextEdit* output; 

public:
    QIodeCommandLine(QWidget *parent = nullptr) : QLineEdit(parent), 
        it(executedCommandsList.end()), c(nullptr), output(nullptr) {}

    void setup(QTextEdit* output, QIodeCompleter* c, const QString& pattern) 
    { 
        this->output = output;
        this->setCompleter(c);
        re.setPattern(pattern);
    }

    QIodeCompleter* completer()
    {
        return c;
    }

    void setCompleter(QIodeCompleter* c)
    {
        if (this->c)
            this->c->disconnect(this);

        this->c = c;

        if (!c)
            return;

        c->setWidget(this);
        // Signal activated is overloaded in QCompleter. 
        // To connect to this signal by using the function pointer syntax, Qt provides 
        // a convenient helper for obtaining the function pointer
        QObject::connect(c, QOverload<const QString &>::of(&QCompleter::activated),
                        this, &QIodeCommandLine::insertCompletion);
    }

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void insertCompletion(const QString& completion);

private:
    QString textUnderCursor();

public slots:
    void run_command();
};
