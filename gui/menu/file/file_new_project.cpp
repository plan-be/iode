#include "file_new_project.h"


MenuFileNewProject::MenuFileNewProject(const QString& currentProjectPath, QWidget* parent) : 
    QDialog(parent)
{
    setupUi(this);

    wDirPath = new WrapperFileChooser(label_location->text(), *fileChooser_new_project, REQUIRED_FIELD, 
                                      DIRECTORY, EXISTING_FILE);
    
    QString parentPath;
    if (currentProjectPath.isEmpty()) 
        parentPath = QDir::homePath();
    else if (currentProjectPath == QDir::homePath())
        parentPath = currentProjectPath;
    else
        parentPath = QFileInfo(currentProjectPath).absolutePath();

    wDirPath->setQValue(parentPath);
}

MenuFileNewProject::~MenuFileNewProject()
{
    delete wDirPath;
}

void MenuFileNewProject::create()
{
    QString newProjectPath = wDirPath->getQValue().toString();
    QFileInfo newProjectInfo(newProjectPath);
    QDir parentDir = newProjectInfo.absoluteDir();
    QString newProjectDirName = newProjectInfo.fileName();
    
    QString error_msg = "Cannot create the new project directory " + newProjectDirName + " in " + newProjectPath;

    // check if the parent dir exists
    if(!parentDir.exists())
    {
        QMessageBox::warning(this, "WARNING", "The directory " + newProjectPath + " does not exist.\n" + error_msg);
        return;
    }

    // check if passed path represents a path to a directory
    if (newProjectInfo.completeSuffix() != "") 
    {
        QMessageBox::warning(this, "WARNING", "The name " + newProjectDirName + " does not represent a directory.\n" + error_msg);
        return;
    }

    // check if new project directory already exists
    if(newProjectInfo.exists())
    {
        QMessageBox::warning(this, "WARNING", "The directory " + newProjectPath + " already exists.\n" + error_msg);
        return;
    }

    try
    {
        parentDir.mkdir(newProjectPath);
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, "WARNING", error_msg + "\n" + QString(e.what()));
        return;
    }

    this->accept();
}
