#include "qiode_command.h"


void QIodeCommandLine::keyPressEvent(QKeyEvent *event)
{
    if(!c.get()) 
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

    switch (event->key())
    {
    case Qt::Key_Up:
        if(executedCommandsList.size() > 0)
        {
            // get previous command from the list
            if(it != executedCommandsList.begin()) --it;
            setText(*it);
        }
        break;
    case Qt::Key_Down:
        if(executedCommandsList.size() > 0)
        {
            // get next command from the list
            if(it != executedCommandsList.end()) ++it;
            // Note: The end() function of a container returns an iterator to 
            //       the imaginary item one position past the last item in the container. 
            //       end() marks an invalid position; it must never be dereferenced.
            if(it == executedCommandsList.end())
                setText("");
            else
                setText(*it);
        }
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        run_command();
        break;
    default:
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
}

void QIodeCommandLine::focusInEvent(QFocusEvent* event)
{
    if (c.get())
        c->setWidget(this);
    QLineEdit::focusInEvent(event);
}

void QIodeCommandLine::insertCompletion(const QString& completion)
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

QString QIodeCommandLine::textUnderCursor()
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

void QIodeCommandLine::run_command()
{
    QString cmd = text();
    if(cmd.isEmpty()) return;

    // ask to compute hash of databases
    askComputeHash(true);

    // add the command to execute in the output panel
    output->append("> " + cmd);
    // reset the input command line
    setText("");

    // try to execute the command
    int success = -1;
    QString error_msg;
    try
    {
        success = B_ReportLine(cmd.toStdString().data());
        if(success == 0)
        {
            if(!executedCommandsList.contains(cmd)) executedCommandsList.append(cmd);
            it = executedCommandsList.constEnd();
            // connected to MainWindow::update_current_tab()
            emit returnPressed();
            // check if databases have changed
            askComputeHash(false);
        }
    }
    catch(const std::exception& e)
    {
        error_msg = QString::fromStdString(e.what());
        QMessageBox::warning(get_main_window_ptr(), "Warning", error_msg + "\n");
    }

    if(success != 0)
    {
        output->append("Failed to execute " + cmd);
        if(!error_msg.isEmpty()) output->append(error_msg);
    } 
}
