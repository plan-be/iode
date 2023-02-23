#include "qiode_tab_widget.h"


QIodeTabWidget::QIodeTabWidget(QWidget* parent) : QTabWidget(parent), overwrite_all(false), discard_all(false)
{
    settings = nullptr;

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
    clearShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), this);

    // ---- file system watcher ----
    fileSystemWatcher = new QFileSystemWatcher(this);
    connect(fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, &QIodeTabWidget::reloadFile);

    // ---- connect signals to slots  ----
    connect(this, &QTabWidget::currentChanged, this, &QIodeTabWidget::showTab);
    connect(this, &QTabWidget::tabCloseRequested, this, &QIodeTabWidget::removeTab);

    connect(nextTabShortcut, &QShortcut::activated, this, &QIodeTabWidget::showNextTab);
    connect(previousTabShortcut, &QShortcut::activated, this, &QIodeTabWidget::showPreviousTab);
    connect(clearShortcut, &QShortcut::activated, this, &QIodeTabWidget::clearCurrentTab);

    // prepare widgets for tabs associated with IODE object types
    tabComments = new QIodeCommentsWidget(I_COMMENTS, this);
    tabEquations = new QIodeEquationsWidget(I_EQUATIONS, this);
    tabIdentites = new QIodeIdentitiesWidget(I_IDENTITIES, this);
    tabLists = new QIodeListsWidget(I_LISTS, this);
    tabScalars = new QIodeScalarsWidget(I_SCALARS, this);
    tabTables = new QIodeTablesWidget(I_TABLES, this);
    tabVariables = new QIodeVariablesWidget(I_VARIABLES, this);

    tabIodeObjects.append(tabComments);
    tabIodeObjects.append(tabEquations);
    tabIodeObjects.append(tabIdentites);
    tabIodeObjects.append(tabLists);
    tabIodeObjects.append(tabScalars);
    tabIodeObjects.append(tabTables);
    tabIodeObjects.append(tabVariables);
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

    delete nextTabShortcut;
    delete previousTabShortcut;
    delete clearShortcut;

    delete fileSystemWatcher;
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
        showTab(0);
    else
    {
        QString projectName = QFileInfo(projectDirPath).fileName();
        QProgressDialog progress("", "", 0, filesToLoad.size());
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowTitle("Loading project " + projectName);
        progress.setCancelButton(nullptr);

        // reopen all tabs (files) that were open the last time the user quitted the IODE gui
        QString filepath;
        QString filename;
        for(int i=0; i < filesToLoad.size(); i++)
        {
            filepath = filesToLoad.at(i);
            filename = QFileInfo(filepath).fileName();

            progress.setLabelText("Loading file " + filename + "...");
            progress.setValue(i);
            QCoreApplication::processEvents();

            // tab associated with a KDB which hasn't been saved the last time the user quitted the IODE gui
            // (i.e. no filepath associated)
            int index;
            if (filepath.startsWith(prefixUnsavedDatabase))
            {
                QString iodeTypeString = filepath.split(" ")[1];
                int iodeType = get_iode_type(iodeTypeString.toStdString());
                // should never happen !
                if (iodeType < 0)
                {
                    QMessageBox::critical(nullptr, "Error", 
                        "loadSettings(): Something went wrong when trying to load " + filepath);
                    return;
                }
                index = updateObjectTab((EnumIodeType) iodeType);
                if(index != i) this->tabBar()->moveTab(index, i);
            }
            else 
                loadFile(filepath, false, true, i);
        }
        progress.setValue(filesToLoad.size());
        
        // display the tab that was displayed the last time the user quitted the IODE gui
        showTab(index);
    }

}
 
void QIodeTabWidget::saveSettings()
{
    // build list of open tabs
    QStringList filesList;
    AbstractTabWidget* tabWidget;
    for (int i=0; i < this->count(); i++)
    {
        tabWidget = static_cast<AbstractTabWidget*>(this->widget(i));
        EnumIodeFile filetype = tabWidget->getFiletype();
        if(filetype <= I_VARIABLES_FILE && static_cast<AbstractIodeObjectWidget*>(tabWidget)->isUnsavedDatabase())
            filesList << prefixUnsavedDatabase + " " + QString::fromStdString(vIodeTypes[filetype]);
        else
            filesList << tabWidget->getFilepath();
    }

    // start to save settings
    settings->beginGroup("Project");

    // save the list of open tabs
    QVariant files = QVariant::fromValue(filesList);
    settings->setValue("files", files);

    // save index of the currently displayed tab
    QVariant index = QVariant(this->currentIndex());
    settings->setValue("index_last_open_tab", index);

    settings->endGroup();
}

void QIodeTabWidget::setup(std::shared_ptr<QString>& project_settings_filepath, 
    std::shared_ptr<QIodeCompleter>& completer, QTextEdit* output)
{
    // save previous settings if any before to switch from project directory
    if (this->project_settings_filepath) saveSettings();

    // set completer
    this->completer = completer;

    // set output
    this->output = output;

    // close all tabs
    this->clear();

    // update settings filepath in each Iode object tab
    tabComments->setProjectSettingsFilepath(project_settings_filepath);
    tabEquations->setProjectSettingsFilepath(project_settings_filepath);
    tabIdentites->setProjectSettingsFilepath(project_settings_filepath);
    tabLists->setProjectSettingsFilepath(project_settings_filepath);
    tabScalars->setProjectSettingsFilepath(project_settings_filepath);
    tabTables->setProjectSettingsFilepath(project_settings_filepath);
    tabVariables->setProjectSettingsFilepath(project_settings_filepath);

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

    // make sure that all KDB tabs has a default text and tooltip
    // (usually when starting a new project)
    for (int index=0; index < this->count(); index++)
    {
        AbstractTabWidget* tabWidget = static_cast<AbstractTabWidget*>(this->widget(index));
        setTabText(index, tabWidget->getTabText());
        setTabToolTip(index, tabWidget->getTooltip());
    }

    this->project_settings_filepath = project_settings_filepath;

    if(settings) delete settings;
    settings = new QSettings(*project_settings_filepath, QSettings::IniFormat);
    loadSettings();

    this->show();
}

void QIodeTabWidget::updateFileSystemWatcher(const QDir& projectDir)
{
    // remove all previously registered directories and files
    fileSystemWatcher->removePaths(fileSystemWatcher->directories());
    fileSystemWatcher->removePaths(fileSystemWatcher->files());

    // register sub-directories and files of the new project directory
    QStringList paths = SystemItem(QFileInfo(projectDir.absoluteFilePath(""))).recursiveEntryList();
    fileSystemWatcher->addPaths(paths);
}

void QIodeTabWidget::resetFilters()
{
    foreach(AbstractIodeObjectWidget* tabWidget, tabIodeObjects) tabWidget->resetFilter();
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
    foreach(AbstractIodeObjectWidget* tabWidget, tabIodeObjects) tabWidget->clearKDB();
    for(int i=0; i < I_NB_TYPES; i++) updateObjectTab((EnumIodeType) i);
}

int QIodeTabWidget::updateObjectTab(const EnumIodeType iodeType)
{
    int index;
    try
    {
        // get index of corresponding tab + reset filter
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

        // extract corresponding tab widget
        AbstractIodeObjectWidget* tabWidget = static_cast<AbstractIodeObjectWidget*>(this->widget(index));

        // reset filter
        tabWidget->resetFilter();

        // update tab text and tooltip
        setTabText(index, tabWidget->getTabText());
        setTabToolTip(index, tabWidget->getTooltip());
    }
    catch (const std::exception& e)
    {
        QWidget* mainwin = get_main_window_ptr();
        QMessageBox::critical(mainwin, tr("ERROR"), tr(e.what()));
        return -1;
    }

    return index;
}

int QIodeTabWidget::addReportTab(const QFileInfo& fileInfo)
{
    QWidget* mainwin = get_main_window_ptr();
    QIodeReportWidget* reportWidget = new QIodeReportWidget(fileInfo.absoluteFilePath(), output, completer, this);
    int index = this->addTab(reportWidget, reportWidget->getTabText());
    setTabToolTip(index, reportWidget->getTooltip());
    return index;
}

int QIodeTabWidget::addTextTab(const QFileInfo& fileInfo, const EnumIodeFile iodeFile)
{
    QWidget* mainwin = get_main_window_ptr();
    QIodeTextWidget* textWidget = new QIodeTextWidget(iodeFile, fileInfo.absoluteFilePath(), this);
    int index = this->addTab(textWidget, textWidget->getTabText());
    setTabToolTip(index, textWidget->getTooltip());
    return index;
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
        index = addTextTab(fileInfo, I_LOGS_FILE);
        break;
    default:
        if (fileInfo.suffix() == "txt" || fileInfo.suffix() == "a2m") 
            index = addTextTab(fileInfo, I_TEXT_FILE);
        else
        { 
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
            index = -1;
        }
    }
    if (index < 0) return index;

    return index;
}

void QIodeTabWidget::removeTab(const int index)
{
    QTabWidget::removeTab(index);
}

void QIodeTabWidget::showTab(int index)
{
    // get the index of the tab currently visible if not passed to the method
    if (index < 0) index = this->currentIndex();

    // get type of file
    QString filename = this->tabText(index);
    EnumIodeFile fileType = get_iode_file_type(filename.toStdString());

    // update widget if needed
    AbstractTabWidget* tabWidget = static_cast<AbstractTabWidget*>(this->widget(index));
    if(tabWidget) tabWidget->update();

    this->setCurrentIndex(index);
}

int QIodeTabWidget::loadFile(const QString& filepath, const bool displayTab, const bool forceOverwrite, const int moveToPosition)
{
    QString filename;
    QString tooltip;
    int index;
    QWidget* mainwin = get_main_window_ptr();
    
	if(filepath.isEmpty())
    {
        QMessageBox::warning(mainwin, "Warning", "Cannot load file because given filepath is empty");
        return -1;
    }

    QFileInfo fileInfo(filepath);
    QString fullPath = fileInfo.absoluteFilePath();

	if (!fileInfo.exists())
	{
		QMessageBox::critical(mainwin, "Error", "File " + fullPath + " has not been found!");
		return -1;
	}

	if (fileInfo.isDir())
	{ 
		QMessageBox::warning(mainwin, "Warning", "Cannot load " + fullPath + " as it is not a file but a directory");
		return -1;
	}

    // check if file already loaded
    QStringList tabs = buildFilesList();
    if(tabs.indexOf(fullPath) >= 0)
        return -1;

    // load file
    try
    {
        EnumIodeFile filetype = get_iode_file_type(fullPath.toStdString());
        // load KDB file
        if(filetype <= I_VARIABLES_FILE)
        {
            AbstractIodeObjectWidget* tabWidget = tabIodeObjects[filetype];
            bool success = tabWidget->load(fullPath, forceOverwrite);
            if(success)
            {
                tabWidget->resetFilter();
                index = this->indexOf(tabWidget);
                setTabText(index, tabWidget->getTabText());
                setTabToolTip(index, tabWidget->getTooltip());
            }
            else
                return -1;
        }
        // load non-KDB file and create a new tab
        else
            index = addNewTab(filetype, fileInfo);

        if (moveToPosition >= 0)
        {
            this->tabBar()->moveTab(index, moveToPosition);
            index = moveToPosition;
        }

        if (index >= 0 && displayTab) showTab(index);

        return index;
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(mainwin, tr("ERROR"), tr(e.what()));
        return -1;
    }
}

void QIodeTabWidget::reloadFile(const QString& filepath)
{
	// find index of corresponding tab
	QStringList tabs = buildFilesList();
	int index = tabs.indexOf(filepath);
	if(index < -1)
		return;

	// reload file
	QString filename = QFileInfo(filepath).fileName();    
	AbstractTabWidget* tabWidget = static_cast<AbstractTabWidget*>(this->widget(index));
	tabWidget->load(filepath, true);
}

bool QIodeTabWidget::saveTabContent(const int index)
{
    try
    {
        AbstractTabWidget* tabWidget = static_cast<AbstractTabWidget*>(this->widget(index));
        QString filepath = tabWidget->save();
        if (filepath.isEmpty()) 
            return false;
        
        setTabText(index, tabWidget->getTabText());
        setTabToolTip(index, tabWidget->getTooltip());
        return true;
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(get_main_window_ptr(), "Warning", "Couldn't save the content of " + tabText(index));
        return false;
    }
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
            updateObjectTab((EnumIodeType) filetype);
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

void QIodeTabWidget::fileMoved(const QString &oldFilepath, const QString &newFilepath)
{
    // check if oldFilePath present in currently open tabs
    QStringList tabs = buildFilesList();
    int index = tabs.indexOf(oldFilepath);
    if (index >= 0)
    {
        AbstractTabWidget* tabWidget = static_cast<AbstractTabWidget*>(this->widget(index));
        // update filename in widget
        if(!tabWidget->updateFilepath(newFilepath))
            return;
        // update name and tooltip of corresponding tab
        setTabText(index, tabWidget->getTabText());
        setTabToolTip(index, tabWidget->getTooltip());
    }
}
