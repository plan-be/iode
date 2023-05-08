#include "iode_command.h"


bool QIodeCommandLine::handleSpecialKeys(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Up:
        if(executedCommandsList.size() > 0)
        {
            // get previous command from the list
            if(it != executedCommandsList.begin()) --it;
            setText(*it);
        }
        return true;
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
        return true;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        run_command();
        return true;
    default:
        break;
    }
    return false;
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
