#pragma once

#include "utils.h"
#include "text_edit/completer.h"

#include <QObject>
#include <QLineEdit>
#include <QCompleter>
#include <QString>
#include <QKeyEvent>
#include <QAbstractItemView>


/**
 * @brief Special line field which tries to auto-complete user input.
 * 
 *        Auto-completion starts at the beginning of the line 
 *        or after a whitespace or any characters which is not 
 *        A-Z, a-z, 0-9, _, $, #, and @. 
 * 
 */
class QIodeCompleteLineEdit: public QLineEdit
{
    Q_OBJECT

protected:
    QIodeCompleter* c;

public:
    QIodeCompleteLineEdit(QWidget *parent = nullptr): QLineEdit(parent), c(nullptr) {}

    QIodeCompleter* getCompleter() const
    {
        return c;
    }

    void setCompleter(QIodeCompleter* c) 
    {
        this->c = c;

        if (!c)
            return;

        c->setWidget(this);
        // Signal activated is overloaded in QCompleter. 
        // To connect to this signal by using the function pointer syntax, Qt provides 
        // a convenient helper for obtaining the function pointer
        connect(c, QOverload<const QString &>::of(&QCompleter::activated),
                this, &QIodeCompleteLineEdit::insertCompletion);
    }

protected:
    void keyPressEvent(QKeyEvent *event) override;

    // event handler used to receive keyboard focus events for the widget.
    void focusInEvent(QFocusEvent* event) override;

    /**
     * @param event 
     * @return bool weither or not to exit keyPressEvent() 
     */
    virtual bool handleSpecialKeys(QKeyEvent *event) 
    {
        return false;
    }

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
