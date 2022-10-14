#include "qiode_tab_widget.h"


QIodeTabWidget::QIodeTabWidget(QWidget* parent) : QTabWidget(parent), overwrite_all(false), discard_all(false)
{
    // set name
    this->setObjectName(QString::fromUtf8("tabWidget_IODE_objs"));

    // set policy
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(sizePolicy);

    // ---- connect signals to slots  ----
    connect(this, &QTabWidget::currentChanged, this, &QIodeTabWidget::viewTab);
    connect(this, &QTabWidget::tabCloseRequested, this, &QIodeTabWidget::removeTab);
}

QIodeTabWidget::~QIodeTabWidget()
{
    saveSettings();
    if(settings) delete settings;
}

void QIodeTabWidget::loadSettings()
{
    // Reload previously opened files
    settings->beginGroup("Project");
    QStringList filesToLoad = settings->value("files", "").toStringList();
    int index = settings->value("index_last_open_tab", "").toInt();
    settings->endGroup();

    for (int i=0; i < filesToLoad.size(); i++) loadFile(filesToLoad.at(i));
    viewTab(index);
}

// TODO : call this method when opening a new/other project 
void QIodeTabWidget::saveSettings()
{
    // build list of open files (= tabs)
    QStringList filesList = buildFilesList();

    // save list to current project settings
    settings->beginGroup("Project");
    QVariant files = QVariant::fromValue(filesList);
    settings->setValue("files", files);
    QVariant index = QVariant(this->currentIndex());
    settings->setValue("index_last_open_tab", index);
    settings->endGroup();
}

void QIodeTabWidget::setup(std::shared_ptr<QString>& project_settings_filepath)
{
    this->project_settings_filepath = project_settings_filepath;
    settings = new QSettings(*project_settings_filepath, QSettings::IniFormat);
    loadSettings();
}

int QIodeTabWidget::addObjectTab(const EnumIodeType iodeType, const QFileInfo& fileInfo)
{
    AbstractIodeObjectWidget* objWidget;
    switch (iodeType)
    {
    case I_COMMENTS:
        objWidget = new QIodeCommentsWidget(project_settings_filepath, iodeType, this);
        break;
    case I_EQUATIONS:
        objWidget = new QIodeEquationsWidget(project_settings_filepath, iodeType, this);
        break;
    case I_IDENTITIES:
        objWidget = new QIodeIdentitiesWidget(project_settings_filepath, iodeType, this);
        break;
    case I_LISTS:
        objWidget = new QIodeListsWidget(project_settings_filepath, iodeType, this);
        break;
    case I_SCALARS:
        objWidget = new QIodeScalarsWidget(project_settings_filepath, iodeType, this);
        break;
    case I_TABLES:
        objWidget = new QIodeTablesWidget(project_settings_filepath, iodeType, this);
        break;
    case I_VARIABLES:
        objWidget = new QIodeVariablesWidget(project_settings_filepath, iodeType, this);
        break;
    default:
        break;
    }

    // add Iode object widget to tabs 
    return this->addTab(objWidget, fileInfo.fileName());
}

// TODO : implement this method
int QIodeTabWidget::addReportTab(const QFileInfo& fileInfo)
{
    QWidget* mainwin = get_main_window_ptr();
    QMessageBox::information(mainwin, "INFORMATION", 
        "Loading of files with extension .rep not (yet) implemented.");
    return -1;
}

// TODO : - add a Save As buttons.
int QIodeTabWidget::addNewTab(const EnumIodeFile fileType, const QFileInfo& fileInfo)
{
    QWidget* mainwin = get_main_window_ptr();
    QString fullPath = fileInfo.absoluteFilePath();

    // Note: rebuild list of open files (= tabs) since users can move tabs
    //       and then change the value returned by indexOf()
    QStringList filesList = buildFilesList();

    // Note: indexOf() returns -1 if not found
    int index = filesList.indexOf(fullPath);

    // check if file already opened
    if (index >= 0) return index;

    // prepare and add the new tab
    switch (fileType)
    {
    case I_COMMENTS_FILE:
        index = addObjectTab(I_COMMENTS, fileInfo);
        break;
    case I_EQUATIONS_FILE:
        index = addObjectTab(I_EQUATIONS, fileInfo);
        break;
    case I_IDENTITIES_FILE:
        index = addObjectTab(I_IDENTITIES, fileInfo);
        break;
    case I_LISTS_FILE:
        index = addObjectTab(I_LISTS, fileInfo);
        break;
    case I_SCALARS_FILE:
        index = addObjectTab(I_SCALARS, fileInfo);
        break;
    case I_TABLES_FILE:
        index = addObjectTab(I_TABLES, fileInfo);
        break;
    case I_VARIABLES_FILE:
        index = addObjectTab(I_VARIABLES, fileInfo);
        break;
    case I_REPORTS_FILE:
        index = addReportTab(fileInfo);
        break;
    default:
        QMessageBox::information(mainwin, "INFORMATION", 
            "Loading of files with extension " + fileInfo.suffix() + " not (yet) implemented.");
    }

    // set tooltip
    // Note: index < 0 if adding new tab has failed
    if (index >= 0) this->setTabToolTip(index, fullPath);

    return index;
}

void QIodeTabWidget::removeTab(const int index)
{
    QTabWidget::removeTab(index);
}

void QIodeTabWidget::resetFilters()
{
    QString filename;
    for (int index=0; index < this->count(); index++)
    {
        filename = this->tabText(index);
        EnumIodeFile fileType = get_iode_file_type(filename.toStdString());
        if(fileType <= I_VARIABLES_FILE)
        {
            AbstractIodeObjectWidget* objWidget = static_cast<AbstractIodeObjectWidget*>(this->widget(index));
            objWidget->resetFilter();
        }
    }
}

// TODO : update this method for report and text files
void QIodeTabWidget::viewTab(int index)
{
    // get the index of the tab currently visible if not passed to the method
    if (index < 0) index = this->currentIndex();

    // get type of file
    QString filename = this->tabText(index);
    EnumIodeFile fileType = get_iode_file_type(filename.toStdString());

    // update widget if needed
    if(fileType <= I_VARIABLES_FILE)
    {
        AbstractIodeObjectWidget* objWidget = static_cast<AbstractIodeObjectWidget*>(this->widget(index));
        objWidget->update();
    }

    this->setCurrentIndex(index);
}

// TODO : - implement loading of Report (text) file
//        - implement loading of any number of files (but only 1 per Iode type)
void QIodeTabWidget::loadFile(const QString& filepath)
{
    QString filename;
    QString tooltip;
    QWidget* mainwin = get_main_window_ptr();
    
    if(filepath.isEmpty()) return;
    else
    {
        QFileInfo fileInfo(filepath);
        if (!fileInfo.exists()) QMessageBox::critical(mainwin, "Error", "File " + fileInfo.absoluteFilePath() + " has not been found!");
        else
        {
            try
            {
                QString fullPath = fileInfo.absoluteFilePath();
                // Guess (Iode) file type
                QString filename = fileInfo.fileName();
                EnumIodeFile fileType = get_iode_file_type(filename.toStdString());
                // IODE objects
                if (fileType <= I_VARIABLES_FILE)
                {
                    // load Iode file
                    std::string full_path = fullPath.toStdString();
                    load_global_kdb((EnumIodeType) fileType, full_path);
                }
                // Prepare new tab
                int index = addNewTab(fileType, fileInfo);
                viewTab(index);
            }
            catch (const std::exception& e)
            {
                QMessageBox::critical(mainwin, tr("ERROR"), tr(e.what()));
                return;
            }
        }
    }
}

// TODO : - implement saving Report files
void QIodeTabWidget::saveFile(const QString& filepath, const bool loop)
{
    QString filename;
    QWidget* mainwin = get_main_window_ptr();

    if(discard_all) return;
    if(filepath.isEmpty()) return;

    QFileInfo fileInfo(filepath);
    if (!overwrite_all && fileInfo.exists())
    {
        QFlags<QMessageBox::StandardButton> buttons;  
        if(loop) buttons = QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::Discard;
        else buttons = QMessageBox::Yes | QMessageBox::Discard;
        QMessageBox::StandardButton answer = QMessageBox::warning(mainwin, "Warning", "File " + 
            fileInfo.fileName() + " already exist!\n Overwrite it ?", buttons, QMessageBox::Yes);
        if(answer == QMessageBox::YesToAll) overwrite_all = true;
        if(answer == QMessageBox::Discard)
        {
            if (loop) discard_all = true;
            return;
        }
    }
    try
    {
        QString fullPath = fileInfo.absoluteFilePath();
        // Guess (Iode) file type
        QString filename = fileInfo.fileName();
        EnumIodeFile fileType = get_iode_file_type(filename.toStdString());
        std::string full_path = fullPath.toStdString();
        // IODE objects
        if (fileType <= I_VARIABLES_FILE)
        {
            // save Iode file
            save_global_kdb((EnumIodeType) fileType, full_path);
        }
        else if (fileType == I_REPORTS_FILE)
        {
            // save Report or text file
            // TODO : implement saving report file
        }
        else QMessageBox::warning(mainwin, tr("Warning"), "Can't save file with extension " + fileInfo.suffix());
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(mainwin, tr("ERROR"), tr(e.what()));
        return;
    }
}

bool QIodeTabWidget::saveProjectAs(QDir& newProjectDir)
{
    QString newProjectPath = newProjectDir.absolutePath();
    QWidget* mainwin = get_main_window_ptr();

    if (!newProjectDir.exists())
    {
        QMessageBox::critical(mainwin, "Error", "Directory " + newProjectPath + " does not exist");
        return false;
    }

    SystemItem currentProject(QFileInfo(projectDirPath), false);
    return currentProject.copyTo(newProjectDir);
}

void QIodeTabWidget::fileRenamed(const QString &path, const QString &oldName, const QString &newName)
{
    QDir parentDir = QDir(path);
    //build absolute path of the file before renaming it
    QString oldFilePath = parentDir.absoluteFilePath(oldName);
    // check if oldFilePath present in currently open tabs
    QStringList tabs = buildFilesList();
    int index = tabs.indexOf(oldFilePath);
    if (index >= 0)
    {
        // update name and tooltip of corresponding tab
        QString newFilePath = parentDir.absoluteFilePath(newName);
        this->setTabText(index, newName);
        this->setTabToolTip(index, newFilePath);
    }
}
