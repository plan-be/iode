#include "tab_widget.h"

QWidget* tabs_widget_ptr = nullptr;


QWidget* get_tabs_widget_ptr()
{
    return tabs_widget_ptr;
}


QIodeTabWidget::QIodeTabWidget(QWidget* parent) : QIodeAbstractTabWidget(parent)
{
    // prepare widgets for tabs associated with IODE object types
    tabComments = new QIodeCommentsWidget(this);
    tabEquations = new QIodeEquationsWidget(this);
    tabIdentites = new QIodeIdentitiesWidget(this);
    tabLists = new QIodeListsWidget(this);
    tabScalars = new QIodeScalarsWidget(this);
    tabTables = new QIodeTablesWidget(this);
    tabVariables = new QIodeVariablesWidget(this);

    connect(tabComments, &QIodeCommentsWidget::tabContentModified, this, &QIodeTabWidget::tabContentModified);
    connect(tabEquations, &QIodeEquationsWidget::tabContentModified, this, &QIodeTabWidget::tabContentModified);
    connect(tabIdentites, &QIodeIdentitiesWidget::tabContentModified, this, &QIodeTabWidget::tabContentModified);
    connect(tabLists, &QIodeListsWidget::tabContentModified, this, &QIodeTabWidget::tabContentModified);
    connect(tabScalars, &QIodeScalarsWidget::tabContentModified, this, &QIodeTabWidget::tabContentModified);
    connect(tabTables, &QIodeTablesWidget::tabContentModified, this, &QIodeTabWidget::tabContentModified);
    connect(tabVariables, &QIodeVariablesWidget::tabContentModified, this, &QIodeTabWidget::tabContentModified);

    tabIodeObjects.append(tabComments);
    tabIodeObjects.append(tabEquations);
    tabIodeObjects.append(tabIdentites);
    tabIodeObjects.append(tabLists);
    tabIodeObjects.append(tabScalars);
    tabIodeObjects.append(tabTables);
    tabIodeObjects.append(tabVariables);

    // ---- global parameters ----
    tabs_widget_ptr = static_cast<QWidget*>(this);
}

QIodeTabWidget::~QIodeTabWidget()
{
    saveSettings();

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
    // reset IODE objects tabs
    foreach(AbstractIodeObjectWidget* tab, tabIodeObjects)
        tab->reset();

    // reset list of open files (tabs)
    filesList.clear();

    // extract settings
    QSettings* project_settings = QIodeProjectSettings::getProjectSettings();
    if(!project_settings)
        return;

    project_settings->beginGroup("PROJECT");
    QStringList filesToLoad = project_settings->value("files").toStringList();
    int index = project_settings->value("index_last_open_tab", "-1").toInt();
    project_settings->endGroup();

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
                    QMessageBox::critical(nullptr, "ERROR", 
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
    QSettings* project_settings = QIodeProjectSettings::getProjectSettings();
    if(!project_settings)
        return;

    // build list of open tabs
    QStringList filesToSave;
    AbstractTabWidget* tabWidget;
    for (int i=0; i < this->count(); i++)
    {
        tabWidget = static_cast<AbstractTabWidget*>(this->widget(i));
        EnumIodeFile filetype = tabWidget->getFiletype();
        if(filetype <= I_VARIABLES_FILE && static_cast<AbstractIodeObjectWidget*>(tabWidget)->isUnsavedDatabase())
            filesToSave << prefixUnsavedDatabase + " " + QString::fromStdString(vIodeTypes[filetype]);
        else
            filesToSave << tabWidget->getFilepath();
    }

    // start to save settings
    project_settings->beginGroup("PROJECT");

    // save the list of open tabs
    QVariant files = QVariant::fromValue(filesToSave);
    project_settings->setValue("files", files);

    // save index of the currently displayed tab
    QVariant index = QVariant(this->currentIndex());
    project_settings->setValue("index_last_open_tab", index);

    project_settings->endGroup();
}

void QIodeTabWidget::setup(std::shared_ptr<QIodeCompleter>& completer, QTextEdit* output)
{
    // set completer
    this->completer = completer;

    // set output
    this->output = output;

    // close all tabs
    this->clear();

    // update settings filepath in each Iode object tab
    tabComments->setup();
    tabEquations->setup();
    tabIdentites->setup();
    tabLists->setup();
    tabScalars->setup();
    tabTables->setup();
    tabVariables->setup();

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

    loadSettings();
    this->show();
}

void QIodeTabWidget::resetFilters()
{
    foreach(AbstractIodeObjectWidget* tabWidget, tabIodeObjects) tabWidget->resetFilter();
}

void QIodeTabWidget::resetFilter(const EnumIodeType iodeType)
{
    AbstractIodeObjectWidget* tabWidget = getIodeObjTab(iodeType);
    if(tabWidget)
        tabWidget->resetFilter();
}

void QIodeTabWidget::clearWorkspace()
{
    foreach(AbstractIodeObjectWidget* tabWidget, tabIodeObjects) tabWidget->clearKDB();
    for(int i=0; i < I_NB_TYPES; i++) updateObjectTab((EnumIodeType) i);
}

QStringList QIodeTabWidget::getSelectedObjectsNames(const EnumIodeType iodeType)
{
        AbstractIodeObjectWidget* tabWidget = getIodeObjTab(iodeType);
        if(tabWidget)
            return tabWidget->getSelectedObjectsNames();
        else
            return QStringList();
}

int QIodeTabWidget::updateObjectTab(const EnumIodeType iodeType)
{
    int index;
    try
    {
        index = getIodeObjTabIndex(iodeType);
        if(index < 0)
            return index;

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
        QMessageBox::critical(nullptr, "ERROR", QString(e.what()));
        return -1;
    }

    return index;
}

int QIodeTabWidget::loadFile(const QString& filepath, const bool displayTab, 
    const bool forceOverwrite, const int moveToPosition)
{
    QString filename;
    QString tooltip;
    int index;
    
	if(filepath.isEmpty())
    {
        QMessageBox::warning(nullptr, "WARNING", "Cannot load file because given filepath is empty");
        return -1;
    }

    QFileInfo fileInfo(filepath);
    QString fullPath = fileInfo.absoluteFilePath();

	if (!fileInfo.exists())
	{
		QMessageBox::warning(nullptr, "WARNING", "File " + fullPath + " has not been found!");
		return -1;
	}

	if (fileInfo.isDir())
	{ 
		QMessageBox::warning(nullptr, "WARNING", "Cannot load " + fullPath + " as it is not a file but a directory");
		return -1;
	}

    // check if file already loaded
    if(filesList.indexOf(fullPath) >= 0)
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
                buildFilesList();
                tabWidget->resetFilter();
                index = this->indexOf(tabWidget);
                setTabText(index, tabWidget->getTabText());
                setTabToolTip(index, tabWidget->getTooltip());
                filesList[index] = fullPath;
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
        QMessageBox::critical(nullptr, "ERROR", QString(e.what()));
        return -1;
    }
}

void QIodeTabWidget::clearCurrentTab()
{
    AbstractTabWidget* tabWidget = static_cast<AbstractTabWidget*>(currentWidget());
    EnumIodeFile filetype = tabWidget->getFiletype();
    if (filetype <= I_VARIABLES_FILE)
    {
        QMessageBox::StandardButton answer = QMessageBox::warning(nullptr, "WARNING", 
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
