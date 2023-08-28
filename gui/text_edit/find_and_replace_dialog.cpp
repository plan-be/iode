#include "find_and_replace_dialog.h"


void FindAndReplaceDialog::onSearchEdited(QString text)
{
    clear();

    if(text.isEmpty())
    {
        labelNbOccurrences->setText("No Results");
        return;
    }

    QTextCursor cursor = editor->textCursor();
    int cursor_pos = cursor.position();
    nbPrevious = 0;
    nbOccurences = 0;

    // constructs a cursor pointing to the beginning of the document.
    QTextCursor doc_cursor(doc);
    // searches for all occurrences of passed text in the whole document
    int current_pos = 0;
    while(!doc_cursor.isNull() && !doc_cursor.atEnd()) 
    {
        doc_cursor = doc->find(text, doc_cursor, QTextDocument::FindWholeWords | QTextDocument::FindCaseSensitively);
        if (!doc_cursor.isNull()) 
        {
            // highlights text
            doc_cursor.mergeCharFormat(allOccurenceFormat);

            // counting occurences
            current_pos = doc_cursor.position();
            nbOccurences++;
            if(current_pos < cursor_pos)
                nbPrevious++;
        }
    }
    
    setLabelNbOccurrences();

    cursor.setPosition(cursor_pos);
    editor->setTextCursor(cursor);
}

bool FindAndReplaceDialog::next()
{
    QTextCursor cursor = editor->textCursor();
    if(cursor.hasSelection())
        cursor.mergeCharFormat(allOccurenceFormat);

    QTextCursor newCursor = doc->find(lineSearch->text(), cursor, QTextDocument::FindWholeWords | QTextDocument::FindCaseSensitively);
    if (!newCursor.isNull()) 
    {
        newCursor.mergeCharFormat(currentOccurenceFormat);
        //QMessageBox::information(nullptr, "next()", "cursor position (after): " + QString::number(newCursor.position()));
        editor->setTextCursor(newCursor);

        nbPrevious++;
        setLabelNbOccurrences();

        return true;
    }
    else
        return false;
}

bool FindAndReplaceDialog::previous()
{
    QTextCursor cursor = editor->textCursor();
    if(cursor.hasSelection())
        cursor.mergeCharFormat(allOccurenceFormat);

    QTextCursor newCursor = doc->find(lineSearch->text(), cursor, 
        QTextDocument::FindWholeWords | QTextDocument::FindCaseSensitively | QTextDocument::FindBackward);
    if (!newCursor.isNull()) 
    {
        newCursor.mergeCharFormat(currentOccurenceFormat);
        editor->setTextCursor(newCursor);

        nbPrevious--;
        setLabelNbOccurrences();

        return true;
    }
    else
        return false;
}

bool FindAndReplaceDialog::replace()
{
    QTextCursor cursor = editor->textCursor();
    if(cursor.hasSelection())
    {
        cursor.removeSelectedText();
        cursor.insertText(lineReplace->text());
        nbOccurences--;

        // reverts nbPrevious++ from the next() method
        if(next())
            nbPrevious--;

        setLabelNbOccurrences();

        return true;
    }
    else
        return false;
}

void FindAndReplaceDialog::replaceAll()
{
    onSearchEdited(lineSearch->text());
    if(nbOccurences == 0) 
        return;

    QTextCursor cursor = editor->textCursor();
    int cursor_pos = cursor.position();

    // set cursor to the beginning of the document
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    editor->setTextCursor(cursor);
    // get first occurrence of text to replace
    next();
    // replace all occurences of text to replace
    while(replace());

    // reset cursor to its orginal position
    cursor.setPosition(cursor_pos);
    editor->setTextCursor(cursor);

    nbPrevious = 0;
    nbOccurences = 0;
    setLabelNbOccurrences();
}

void FindAndReplaceDialog::findOnly(bool find_only)
{
    if(find_only)
    {
        lineReplace->hide();
        replaceButton->hide();
        replaceAllButton->hide();
        resize(width(), lineSearch->height() + 20);
    }
    else
    {
        lineReplace->setHidden(false);
        replaceButton->setHidden(false);
        replaceAllButton->setHidden(false);
        resize(width(), fullHeight);
    }
    lineSearch->setFocus();
}
