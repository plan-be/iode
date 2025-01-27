#include "complete_text_edit.h"


void IodeAutoCompleteTextEdit::keyPressEvent(QKeyEvent *event)
{
    if(!c)
    {
        QPlainTextEdit::keyPressEvent(event);
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
            QPlainTextEdit::keyPressEvent(event);
            return;
       default:
           break;
       }
    }

    // CTRL + Space forces auto-completion
    bool completeShortcut = event->keyCombination() == keyCombineComplete;
    // see https://doc.qt.io/qt-6/qt.html#Key-enum for maximal value of letter + digit key
    if(!completeShortcut && event->key() > 0x0ff)
        return QPlainTextEdit::keyPressEvent(event);

    QPlainTextEdit::keyPressEvent(event);
    
    c->setCompletionPrefix(textUnderCursor());
    if(completeShortcut || c->completionPrefix().size() > 0)
    {
        QRect cr = cursorRect();
        cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
        c->complete(cr);
    }
    else
        c->popup()->hide();
}

void IodeAutoCompleteTextEdit::focusInEvent(QFocusEvent* event)
{
    if(c)
        c->setWidget(this);
    QPlainTextEdit::focusInEvent(event);
}

void IodeAutoCompleteTextEdit::insertCompletion(const QString& completion)
{
    if (c->widget() != this)
        return;

    QTextCursor tc = textCursor();

    // delete completion prefix
    for(int i=0; i < c->completionPrefix().size(); i++) tc.deletePreviousChar();

    // replaces with full function or Iode object name
    tc.insertText(completion);

    // set cursor after introduced function or Iode object name
    setTextCursor(tc);
}

QString IodeAutoCompleteTextEdit::textUnderCursor()
{
    QTextCursor tc = textCursor();
    int pos = tc.position();

    // select the whole line
    tc.select(QTextCursor::BlockUnderCursor);
    QString block = tc.selectedText();
    if(block.isEmpty()) return "";

    // NOTE: tc.select move the cursor to the end of the selection.
    //       we need to move it back to its original position
    tc.setPosition(pos);

    // selects the "word" under cursor
    // WARNING: tc.select(QTextCursor::WordUnderCursor) selects only
    //          alpha characters (i.e. the selection will not include 
    //          characters such as $, # and @)
    tc.select(QTextCursor::WordUnderCursor);
    QString word = tc.selectedText();

    // NOTE: we need to add the lines below because we need to know
    //       if there is a character $, # or @ before the "word"
    tc.movePosition(QTextCursor::StartOfWord);
    pos = tc.positionInBlock();
    QChar::Category cat_first_char = block[0].category();
    if(!(cat_first_char == QChar::Separator_Line || cat_first_char == QChar::Separator_Paragraph) && pos > 0)
        pos--;
    QChar ch = block[pos];
    QString completionPrefix = (ch == '$' || ch == '#' || ch == '@') ? ch + word : word;

    return completionPrefix;
}
