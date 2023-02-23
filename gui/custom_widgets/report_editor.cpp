#include "report_editor.h"


ReportEditor::ReportEditor(std::shared_ptr<QIodeCompleter>& c, QTextEdit* output, QWidget *parent) 
    : TextEditor(parent), output(output)
{
    highlighter = new QIodeHighlighter(this->document());

    this->c = c;

    if (!c.get())
        return;

    c->setWidget(this);
    // Signal activated is overloaded in QCompleter. 
    // To connect to this signal by using the function pointer syntax, Qt provides 
    // a convenient helper for obtaining the function pointer
    connect(c.get(), QOverload<const QString &>::of(&QCompleter::activated),
            this, &ReportEditor::insertCompletion);
}

ReportEditor::~ReportEditor()
{
    delete highlighter;
}

QString ReportEditor::textUnderCursor() const
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
    QChar ch = block[tc.positionInBlock()];
    QString completionPrefix = (ch == '$' || ch == '#' || ch == '@') ? ch + word : word;

    return completionPrefix;
}

void ReportEditor::insertCompletion(const QString& completion)
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

void ReportEditor::keyPressEvent(QKeyEvent* e)
{
    if(!c.get())
    {
        TextEditor::keyPressEvent(e);
        return;
    }

    if(c->popup()->isVisible()) 
    {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) 
       {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       case Qt::Key_Space:
            c->popup()->hide();
            TextEditor::keyPressEvent(e);
            return;
       default:
           break;
       }
    }

    // CTRL + Space forces auto-completion
    bool completeShortcut = e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_Space;
    if(!completeShortcut)
        TextEditor::keyPressEvent(e);
    
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

void ReportEditor::focusInEvent(QFocusEvent* e)
{
    if (c.get())
        c->setWidget(this);
    TextEditor::focusInEvent(e);
}

void ReportEditor::run(const QString& filepath)
{
    QString msg;
    int success = -1;

    try
    {
        output->setTextColor(Qt::darkBlue);
        output->append("\nRunning report " + filepath + " ...\n");
        output->setTextColor(Qt::black);
        success = B_ReportExec(filepath.toStdString().data());
        if(success != 0)
            B_display_last_error();
    }
    catch(const std::exception& e)
    {
        msg = QString::fromStdString(e.what());
        QMessageBox::warning(get_main_window_ptr(), "Warning", msg + "\n");
    }

    if(success == 0)
    {
        output->setTextColor(Qt::darkGreen);
        msg = "Successful";
    }
    else
    {
        output->setTextColor(Qt::red);
        msg = "Failed";
    }
    output->append("\nExecution of report " + filepath + ": " + msg + "\n");
    output->setTextColor(Qt::black);
}