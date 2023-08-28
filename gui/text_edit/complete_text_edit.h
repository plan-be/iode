#pragma once

#include "utils.h"
#include "completer.h"

#include <QObject>
#include <QString>
#include <QKeyEvent>
#include <QCompleter>
#include <QScrollBar>
#include <QPlainTextEdit>
#include <QAbstractItemView>


/**
 * @brief Special multiline field which tries to auto-complete user input.
 * 
 *        Auto-completion starts at the beginning of the line 
 *        or after a whitespace or any characters which is not 
 *        A-Z, a-z, 0-9, _, $, #, and @. 
 * 
 */
class IodeAutoCompleteTextEdit: public QPlainTextEdit
{
    Q_OBJECT

protected:
    IodeCompleter* c;
    QKeyCombination keyCombineComplete;

public:
    IodeAutoCompleteTextEdit(QWidget *parent = nullptr) 
        : QPlainTextEdit(parent), c(nullptr), keyCombineComplete(QKeyCombination(Qt::ControlModifier, Qt::Key_Space)) {}

    IodeCompleter* getCompleter() const
    {
        return c;
    }

    void setCompleter(IodeCompleter* completer) 
    {
        if(c)
            c->disconnect(this);

        c = completer;

        if (!c)
            return;

        c->setWidget(this);
        // Signal activated is overloaded in QCompleter. 
        // To connect to this signal by using the function pointer syntax, Qt provides 
        // a convenient helper for obtaining the function pointer
        connect(c, QOverload<const QString &>::of(&QCompleter::activated),
                this, &IodeAutoCompleteTextEdit::insertCompletion);
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
    void setIodeType(const int iodeType)
    {
        if(c)
            c->setIodeType(iodeType);
    }

    void includeIodeCommand(const bool ok)
    {
        if(c)
            c->includeIodeCommand(ok);
    }

    void includeLecFunctions(const bool ok)
    {
        if(c)
            c->includeLecFunctions(ok);
    }
};
