#include "complete_line_edit.h"


void IodeAutoCompleteLineEdit::keyPressEvent(QKeyEvent *event)
{
    if(!c) 
    {
        QLineEdit::keyPressEvent(event);
        return;
    }

    if(c->popup()->isVisible()) 
    {
        // The following keys are forwarded by the completer to the widget
       switch (event->key()) 
       {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            event->ignore();
            return; // let the completer do default behavior
       case Qt::Key_Space:
            c->popup()->hide();
            QLineEdit::keyPressEvent(event);
            return;
       default:
           break;
       }
    }

    bool exit = handleSpecialKeys(event);
    if(exit)
        return;

    // CTRL + Space forces auto-completion
    bool completeShortcut = event->modifiers().testFlag(Qt::ControlModifier) && event->key() == Qt::Key_Space;
    if(!completeShortcut)
        QLineEdit::keyPressEvent(event);
    
    c->setCompletionPrefix(textUnderCursor());
    if(completeShortcut || c->completionPrefix().size() > 0)
        c->complete();
    else
        c->popup()->hide();
}

void IodeAutoCompleteLineEdit::focusInEvent(QFocusEvent* event)
{
    if(c)
        c->setWidget(this);
    QLineEdit::focusInEvent(event);
}

void IodeAutoCompleteLineEdit::insertCompletion(const QString& completion)
{
    if (c->widget() != this)
        return;
    
    QString str = this->text();
    if(str.size() == 0) return;

    // delete completion prefix
    for(int i=0; i < c->completionPrefix().size(); i++) backspace();

    // replaces with full function or Iode object name
    insert(completion);
}

QString IodeAutoCompleteLineEdit::textUnderCursor()
{
    QString line = this->text().trimmed();
    if(line.size() == 0) return "";

    int pos = cursorPosition();

    // selects the "word" under cursor
    // WARNING: cursorWordBackward(mark) selects only
    //          alpha characters (i.e. the selection will not include 
    //          characters such as $, # and @)
    cursorWordBackward(true);

    // NOTE: we need to add the lines below because we need to know
    //       if there is a character $, # or @ before the "word"
    if(cursorPosition() > 0)
    {
        QChar ch = line[cursorPosition()-1];
        // cursorBackward(mark): Moves the cursor back. If mark is true 
        // each character moved over is added to the selection
        if(ch == '$' || ch == '#' || ch == '@') 
            cursorBackward(true);
    }

    QString str = selectedText();
    deselect();
    setCursorPosition(pos);
    return str;
}
