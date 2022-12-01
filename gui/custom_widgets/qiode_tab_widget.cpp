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

    // prepare shortcuts
    nextTabShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Tab), this);
    previousTabShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab), this);
    saveShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    saveAllShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S), this);
    clearShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), this);

    // ---- connect signals to slots  ----
    connect(this, &QTabWidget::currentChanged, this, &QIodeTabWidget::showTab);
    connect(this, &QTabWidget::tabCloseRequested, this, &QIodeTabWidget::removeTab);
    connect(nextTabShortcut, &QShortcut::activated, this, &QIodeTabWidget::showNextTab);
    connect(previousTabShortcut, &QShortcut::activated, this, &QIodeTabWidget::showPreviousTab);
    connect(saveShortcut, &QShortcut::activated, this, &QIodeTabWidget::saveCurrentTab);
    connect(saveAllShortcut, &QShortcut::activated, this, &QIodeTabWidget::saveAllTabs);
    connect(clearShortcut, &QShortcut::activated, this, &QIodeTabWidget::clearCurrentTab);

    // prepare widgets for tabs associated with IODE object types
    tabComments = new QIodeCommentsWidget(project_settings_filepath, I_COMMENTS, this);
    tabEquations = new QIodeEquationsWidget(project_settings_filepath, I_EQUATIONS, this);
    tabIdentites = new QIodeIdentitiesWidget(project_settings_filepath, I_IDENTITIES, this);
    tabLists = new QIodeListsWidget(project_settings_filepath, I_LISTS, this);
    tabScalars = new QIodeScalarsWidget(project_settings_filepath, I_SCALARS, this);
    tabTables = new QIodeTablesWidget(project_settings_filepath, I_TABLES, this);
    tabVariables = new QIodeVariablesWidget(project_settings_filepath, I_VARIABLES, this);

    // tab prefix per file type
    // Note: If the tab's label contains an ampersand, the letter following the ampersand is used as a shortcut for the tab, 
    //       e.g. if the label is "Bro&wse" then Alt+W becomes a shortcut which will move the focus to this tab.
    //       See https://doc.qt.io/qt-6/qtabwidget.html#addTab 
    tabPrefix.resize(I_NB_FILE_EXT);
    tabPrefix[I_COMMENTS_FILE] = "(&CMT) ";
    tabPrefix[I_EQUATIONS_FILE] = "(&EQS) ";
    tabPrefix[I_IDENTITIES_FILE] = "(&IDT) ";
    tabPrefix[I_LISTS_FILE] = "(&LST) ";
    tabPrefix[I_SCALARS_FILE] = "(&SCL) ";
    tabPrefix[I_TABLES_FILE] = "(&TBL) ";
    tabPrefix[I_VARIABLES_FILE] = "(&VAR) ";
    tabPrefix[I_REPORTS_FILE] = "(REP) ";
    tabPrefix[I_LOGS_FILE] = "(LOG) ";
}

QIodeTabWidget::~QIodeTabWidget()
{
    saveSettings();
    if(settings) delete settings;

    delete tabComments;
    delete tabEquations;
    delete tabIdentites;
    delete tabLists;
    delete tabScalars;
    delete tabTables;
    delete tabVariables;
}

void QIodeTabWidget::loadSettings()
{
    // extract settings
    settings->beginGroup("Project");
    QStringList filesToLoad = settings->value("files", "").toStringList();
    int index = settings->value("index_last_open_tab", "-1").toInt();
    settings->endGroup();

    // filesToLoad is empty in the case of a new project
    if(filesToLoad.empty())
    {
        for (int iodeType=0; iodeType < I_NB_TYPES; iodeType++) 
            setTabNameTooltip(iodeType, (EnumIodeFile) iodeType, "");

        showTab(0);
    }
    else
    {
        // reopen all tabs (files) that were open the last time the user quitted the IODE gui
        QString filepath;
        for(int i=0; i < filesToLoad.size(); i++)
        {
            filepath = filesToLoad.at(i);
            // tab associated with a KDB which hasn't been saved the last time the user quitted the IODE gui
            // (i.e. no filepath associated)
            if (isUnsavedKDB(filepath))
            {
                int iodeType = extractIodeTypeFromDefaultTooltip(filepath);
                // should never happen !
                if (iodeType < 0)
                {
                    QMessageBox::critical(nullptr, "Error", 
                        "loadSettings(): Something went wrong when trying to load " + filepath);
                    return;
                }
                updateObjectTab((EnumIodeType) iodeType, i);
            }
            else 
                loadFile(filepath, false, true, i);
        }
        
        // display the tab that was displayed the last time the user quitted the IODE gui
        showTab(index);
    }

}
 
void QIodeTabWidget::saveSettings()
{
    // build list of open tabs
    QStringList filesList = buildFilesList();

    settings->beginGroup("Project");

    // save the list of open tabs
    QVariant files = QVariant::fromValue(filesList);
    settings->setValue("files", files);

    // save index of the currently displayed tab
    QVariant index = QVariant(this->currentIndex());
    settings->setValue("index_last_open_tab", index);

    settings->endGroup();
}

void QIodeTabWidget::setup(std::shared_ptr<QString>& project_settings_filepath)
{
    // save previous settings if any before to switch from project directory
    if (this->project_settings_filepath) saveSettings();

    // close all tabs
    this->clear();

    // add a default tab for each IODE type of objects
    this->addTab(tabComments, "");
    this->addTab(tabEquations, "");
    this->addTab(tabIdentites, "");
    this->addTab(tabLists, "");
    this->addTab(tabScalars, "");
    this->addTab(tabTables, "");
    this->addTab(tabVariables, "");

    // remove close button for IODE objects tabs
    for(int i = 0; i < this->count(); i++)
    {
        this->tabBar()->tabButton(i, QTabBar::RightSide)->deleteLater();
        this->tabBar()->setTabButton(i, QTabBar::RightSide, nullptr);
    }

    this->project_settings_filepath = project_settings_filepath;
    settings = new QSettings(*project_settings_filepath, QSettings::IniFormat);
    loadSettings();
}

void QIodeTabWidget::resetFilters()
{
    tabComments->resetFilter();
    tabEquations->resetFilter();
    tabIdentites->resetFilter();
    tabLists->resetFilter();
    tabScalars->resetFilter();
    tabTables->resetFilter();
    tabVariables->resetFilter();
}

void QIodeTabWidget::resetFilter(const EnumIodeType iodeType)
{
    switch (iodeType)
    {
    case I_COMMENTS:
        tabComments->resetFilter();
        break;
    case I_EQUATIONS:
        tabEquations->resetFilter();
        break;
    case I_IDENTITIES:
        tabIdentites->resetFilter();
        break;
    case I_LISTS:
        tabLists->resetFilter();
        break;
    case I_SCALARS:
        tabScalars->resetFilter();
        break;
    case I_TABLES:
        tabTables->resetFilter();
        break;
    case I_VARIABLES:
        tabVariables->resetFilter();
        break;
    default:
        break;
    }
}

void QIodeTabWidget::clearWorkspace()
{
    tabComments->clearKDB();
    tabEquations->clearKDB();
    tabIdentites->clearKDB();
    tabLists->clearKDB();
    tabScalars->clearKDB();
    tabTables->clearKDB();
    tabVariables->clearKDB();
    resetFilters();
}

int QIodeTabWidget::updateObjectTab(const EnumIodeType iodeType, const int moveToPosition)
{
    int index;
    try
    {
        // get index of corresponding tab
        switch (iodeType)
        {
        case I_COMMENTS:
            index = this->indexOf(tabComments);
            break;
        case I_EQUATIONS:
            index = this->indexOf(tabEquations);
            break;
        case I_IDENTITIES:
            index = this->indexOf(tabIdentites);
            break;
        case I_LISTS:
            index = this->indexOf(tabLists);
            break;
        case I_SCALARS:
            index = this->indexOf(tabScalars);
            break;
        case I_TABLES:
            index = this->indexOf(tabTables);
            break;
        case I_VARIABLES:
            index = this->indexOf(tabVariables);
            break;
        default:
            return -1;
        }

        // get path to the file associated with KDB of objects of type iodeType
        std::string filepath = std::string(K_get_kdb_nameptr(K_WS[iodeType]));

        // KDB not saved in any file
        if (filepath.empty() || filepath == std::string(I_DEFAULT_FILENAME))
            setTabNameTooltip(index, (EnumIodeFile) iodeType, "");
        else
        {
            // check path and add extension if needed
            filepath = check_filepath(filepath, (EnumIodeFile) iodeType, "tab " + vIodeTypes[iodeType], false);
            setTabNameTooltip(index, (EnumIodeFile) iodeType, QString::fromStdString(filepath));
        }
    }
    catch (const std::exception& e)
    {
        QWidget* mainwin = get_main_window_ptr();
        QMessageBox::critical(mainwin, tr("ERROR"), tr(e.what()));
        return -1;
    }

    if (moveToPosition >= 0)
    {
        this->tabBar()->moveTab(index, moveToPosition);
        index = moveToPosition;
    }

    return index;
}

// TODO : implement this method
int QIodeTabWidget::addReportTab(const QFileInfo& fileInfo)
{
    QWidget* mainwin = get_main_window_ptr();
    QMessageBox::information(mainwin, "INFORMATION", "Cannot load file " + fileInfo.fileName() + ".\n" +
        "Loading of files with extension .rep not (yet) implemented.");
    return -1;
}

// TODO : implement this method
int QIodeTabWidget::addTextTab(const QFileInfo& fileInfo)
{
    QWidget* mainwin = get_main_window_ptr();
    QMessageBox::information(mainwin, "INFORMATION", "Cannot load file " + fileInfo.fileName() + ".\n" +
        "Loading of files with extension " + fileInfo.suffix() + " not (yet) implemented.");
    return -1;
}

// TODO : - add a Save As buttons.
int QIodeTabWidget::addNewTab(const EnumIodeFile fileType, const QFileInfo& fileInfo)
{
    if (fileType <= I_VARIABLES_FILE) return -1;

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
    case I_REPORTS_FILE:
        index = addReportTab(fileInfo);
        break;
    case I_LOGS_FILE:
        index = addTextTab(fileInfo);
        break;
    case I_SETTINGS_FILE:
        index = addTextTab(fileInfo);
        break;
    default:
        if (fileInfo.suffix() == "txt") index = addTextTab(fileInfo);
        else QMessageBox::information(mainwin, "INFORMATION", "Cannot load file " + fileInfo.fileName() + ".\n" +
                "Loading of files with extension " + fileInfo.suffix() + " not (yet) implemented.");
    }
    if (index < 0) return index;

    // set text and tooltip
    setTabNameTooltip(index, fileType, fileInfo.absoluteFilePath());

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
        filename = QFileInfo(this->tabToolTip(index)).fileName();
        EnumIodeFile fileType = get_iode_file_type(filename.toStdString());
        if(fileType <= I_VARIABLES_FILE)
        {
            AbstractIodeObjectWidget* objWidget = static_cast<AbstractIodeObjectWidget*>(this->widget(index));
            objWidget->resetFilter();
        }
    }
}

// TODO : update this method for report and text files
void QIodeTabWidget::showTab(int index)
{
    // get the index of the tab currently visible if not passed to the method
    if (index < 0) index = this->currentIndex();

    // get type of file
    QString filename = this->tabText(index);
    EnumIodeFile fileType = get_iode_file_type(filename.toStdString());

    // update widget if needed
    if(fileType <= I_VARIABLES_FILE)
    {
        AbstractTabWidget* objWidget = static_cast<AbstractTabWidget*>(this->widget(index));
        objWidget->update();
    }

    this->setCurrentIndex(index);
}

// TODO : - implement loading of Report, log and text files
int QIodeTabWidget::loadFile(const QString& filepath, const bool displayTab, const bool forceOverwrite, const int moveToPosition)
{
    QString filename;
    QString tooltip;
    int index;
    QWidget* mainwin = get_main_window_ptr();
    
    if(filepath.isEmpty()) 
        return -1;
    else
    {
        QFileInfo fileInfo(filepath);
        if (!fileInfo.exists())
        {
            QMessageBox::critical(mainwin, "Error", "File " + fileInfo.absoluteFilePath() + " has not been found!");
            return -1;
        }
        else
        {
            try
            {
                QString fullPath = fileInfo.absoluteFilePath();
                // Guess (Iode) file type
                QString filename = fileInfo.fileName();
                EnumIodeFile fileType = get_iode_file_type(filename.toStdString());
                // IODE objects
                if (fileType == I_DIRECTORY)
                { 
                    QMessageBox::warning(mainwin, "Warning", "Cannot load " + fullPath + " as it is not a file but a directory");
                    return index;
                }
                else if (fileType <= I_VARIABLES_FILE)
                {
                    const EnumIodeType iodeType = (EnumIodeType) fileType;
                    // check if correponding K_WS is not empty (except if force overwrite -> loadSettings())
                    if(!forceOverwrite && is_iode_objs_loaded(iodeType))
                    {
                        QString iodeTypeName = QString::fromStdString(vIodeTypes[fileType]);
                        QMessageBox::StandardButton answer = QMessageBox::warning(mainwin, "Warning", "There are " + 
                            iodeTypeName + " already loaded. Would like to override " + iodeTypeName + " ?", 
                            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                        if(answer == QMessageBox::No) return index;
                    }
                    // load Iode file
                    std::string full_path = fullPath.toStdString();
                    load_global_kdb(iodeType, full_path);
                    index = updateObjectTab(iodeType, moveToPosition);
                }
                // Prepare new tab (or simply return corresponding index if file already open)
                else
                    index = addNewTab(fileType, fileInfo);

                if (displayTab) showTab(index);
                return index;
            }
            catch (const std::exception& e)
            {
                QMessageBox::critical(mainwin, tr("ERROR"), tr(e.what()));
                return -1;
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

bool QIodeTabWidget::saveTabContent(const int index)
{
    AbstractTabWidget* tabWidget = static_cast<AbstractTabWidget*>(this->widget(index));
    QString filepath = tabWidget->save();
    if (filepath.isEmpty()) return false;
    setTabNameTooltip(index, tabWidget->getFiletype(), filepath);
    return true;
}

bool QIodeTabWidget::saveCurrentTab()
{
    int index = currentIndex();
    return saveTabContent(index);
}

bool QIodeTabWidget::saveAllTabs()
{
    bool success = true;
    for(int index=0; index < this->count(); index++)
        if(!saveTabContent(index)) success = false;
    return success;
}

void QIodeTabWidget::clearCurrentTab()
{
    AbstractTabWidget* tabWidget = static_cast<AbstractTabWidget*>(currentWidget());
    EnumIodeFile filetype = tabWidget->getFiletype();
    if (filetype <= I_VARIABLES_FILE)
    {
        QWidget* mainwin = get_main_window_ptr();
        QMessageBox::StandardButton answer = QMessageBox::warning(mainwin, "Warning", 
            "Are you sure to clear the " + QString::fromStdString(vIodeTypes[filetype]) + " database", 
            QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
        if (answer == QMessageBox::Yes)
        {
            AbstractIodeObjectWidget* kdbWidget = static_cast<AbstractIodeObjectWidget*>(tabWidget);
            kdbWidget->clearKDB();
            kdbWidget->resetFilter();
        }
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
        QString newFilePath = parentDir.absoluteFilePath(newName);
        EnumIodeFile filetype = get_iode_file_type(newFilePath.toStdString());
        // update name and tooltip of corresponding tab
        setTabNameTooltip(index, filetype, newFilePath);
        // if file associated with KDB object -> update filename of KDB
        if (filetype <= I_VARIABLES_FILE) set_kdb_filename(K_WS[filetype], newFilePath.toStdString());
    }
}
