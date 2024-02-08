#include "iode_command.h"


bool IodeCommandLine::handleSpecialKeys(QKeyEvent *event)
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

void IodeCommandLine::saveSettings()
{
    QSettings user_settings(QSettings::UserScope, this);
    user_settings.beginGroup(SETTINGS_GROUP_NAME);
    
    int nbCommandsToSave = qMin(executedCommandsList.size(), MAX_NB_COMMANDS_TO_REMEMBER);
    QStringList::const_iterator it_ = executedCommandsList.constEnd();
    it_ -= nbCommandsToSave;
    QStringList commandsToSave(it_, executedCommandsList.constEnd());
    user_settings.setValue("LAST_EXECUTED_COMMANDS", commandsToSave);
    
    user_settings.endGroup();

}

void IodeCommandLine::loadSettings()
{
    QSettings user_settings(QSettings::UserScope, this);
    user_settings.beginGroup(SETTINGS_GROUP_NAME);
    
    executedCommandsList = user_settings.value("LAST_EXECUTED_COMMANDS", QStringList()).toStringList();
    it = executedCommandsList.end();
    
    user_settings.endGroup();
}

void IodeCommandLine::run_command()
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
    bool success;
    QString error_msg;
    try
    {
        execute_report_line(cmd.toStdString());
        success = true;
    }
    catch(const std::exception& e)
    {
        success = false;
        error_msg = QString::fromStdString(e.what());
        QMessageBox::warning(nullptr, "WARNING", error_msg + "\n");
    }

    if(success)
    {
        if(!executedCommandsList.contains(cmd)) executedCommandsList.append(cmd);
        it = executedCommandsList.constEnd();
        // connected to MainWindow::update_tab_and_completer()
        emit returnPressed();
    }
    else
    {
        output->append("Failed to execute " + cmd);
        if(!error_msg.isEmpty()) output->append(error_msg);
    } 
}
