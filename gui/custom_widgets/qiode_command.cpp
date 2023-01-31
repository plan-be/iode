#include "qiode_command.h"


void QIodeCommandLine::keyPressEvent(QKeyEvent *event)
{
    QString nb_commands = QString::number(executedCommandsList.size());

    if (c && c->popup()->isVisible()) 
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
        QLineEdit::keyPressEvent(event);

        if(!c) return;
        c->setCompletionPrefix(textUnderCursor());
        if (c->completionPrefix().size() < 1)
        {
            c->popup()->hide();
            return;
        } 
        c->complete();
        break;
    }
}

void QIodeCommandLine::insertCompletion(const QString& completion)
{
    if (c->widget() != this)
        return;
    
    QString str = this->text();
    if(str.size() == 0) return;

    int start = str.lastIndexOf(re) + 1;
    if(start < 0)
        this->setText(completion);
    else
        this->setText(str.first(start) + completion);
}

QString QIodeCommandLine::textUnderCursor()
{
    QString str = this->text();
    if(str.size() == 0) return "";

    QStringList words = str.split(re);
    return words.last();
}

void QIodeCommandLine::run_command()
{
    QString cmd = text();
    if(cmd.isEmpty()) return;

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
