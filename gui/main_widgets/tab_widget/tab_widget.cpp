#include "tab_widget.h"

QWidget* tabs_widget_ptr = nullptr;


QWidget* get_tabs_widget_ptr()
{
    return tabs_widget_ptr;
}


IodeTabWidget::IodeTabWidget(QWidget* parent) : IodeAbstractTabWidget(parent)
{
    // global parameters
    // NOTE: must initialized first since the function get_tabs_widget_ptr() is called 
    //       in the constructor of the CommentsWidget, ... and VariablesWidget classes
    tabs_widget_ptr = static_cast<QWidget*>(this);

    // prepare widgets for tabs associated with IODE object types
    tabComments  = new CommentsWidget(this);
    tabEquations = new EquationsWidget(this);
    tabIdentites = new IdentitiesWidget(this);
    tabLists     = new ListsWidget(this);
    tabScalars   = new ScalarsWidget(this);
    tabTables    = new TablesWidget(this);
    tabVariables = new VariablesWidget(this);

    connect(tabComments, &CommentsWidget::tabDatabaseModified, this, &IodeTabWidget::tabDatabaseModified);
    connect(tabEquations, &EquationsWidget::tabDatabaseModified, this, &IodeTabWidget::tabDatabaseModified);
    connect(tabIdentites, &IdentitiesWidget::tabDatabaseModified, this, &IodeTabWidget::tabDatabaseModified);
    connect(tabLists, &ListsWidget::tabDatabaseModified, this, &IodeTabWidget::tabDatabaseModified);
    connect(tabScalars, &ScalarsWidget::tabDatabaseModified, this, &IodeTabWidget::tabDatabaseModified);
    connect(tabTables, &TablesWidget::tabDatabaseModified, this, &IodeTabWidget::tabDatabaseModified);
    connect(tabVariables, &VariablesWidget::tabDatabaseModified, this, &IodeTabWidget::tabDatabaseModified);

    tabIodeObjects.append(tabComments);
    tabIodeObjects.append(tabEquations);
    tabIodeObjects.append(tabIdentites);
    tabIodeObjects.append(tabLists);
    tabIodeObjects.append(tabScalars);
    tabIodeObjects.append(tabTables);
    tabIodeObjects.append(tabVariables);
}

IodeTabWidget::~IodeTabWidget()
{
    saveSettings();

    Comments.clear();
    Equations.clear();
    Identities.clear();
    Lists.clear();
    Scalars.clear();
    Tables.clear();
    Variables.clear();

    delete tabComments;
    delete tabEquations;
    delete tabIdentites;
    delete tabLists;
    delete tabScalars;
    delete tabTables;
    delete tabVariables;
}

void IodeTabWidget::loadSettings()
{
    // reset IODE objects tabs
    foreach(AbstractIodeObjectWidget* tab, tabIodeObjects)
        tab->reset();

    // reset list of open files (tabs)
    filesList.clear();

    // extract settings
    QSettings* project_settings = ProjectSettings::getProjectSettings();
    if(!project_settings)
        return;
    
    // get project name (which is the name of the root directory of the project)
    QString projectName = QFileInfo(projectDirPath).fileName();

    // ---- LOAD SETTINGS ----
    project_settings->beginGroup("PROJECT");

    int index = -1;
    QString filepath;
    IodeFileType filetype;
    bool forcedAsText;
    bool splitted;
    Qt::Orientation splitOrientation;

    int size = project_settings->beginReadArray("tabs");
    if(size == 0)
        showTab(0);
    else
    {
        QProgressDialog progress("", "", 0, size);
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowTitle("Loading project " + projectName);
        progress.setCancelButton(nullptr);

        QString filename;
        // reopen all tabs (files) that were open the last time the user quitted the IODE gui
        for (int i = 0; i < size; i++) 
        {
            project_settings->setArrayIndex(i);

            filepath = project_settings->value("filepath").toString();
            filetype = (IodeFileType) project_settings->value("filetype").toInt();
            forcedAsText = project_settings->value("forcedAsText").toBool();
            splitted = project_settings->value("splitted").toBool();
            splitOrientation = (Qt::Orientation) project_settings->value("splitOrientation").toInt();
            
            filename = QFileInfo(filepath).fileName();

            progress.setLabelText("Loading file " + filename + "...");
            progress.setValue(i);
            QCoreApplication::processEvents();

            // Tab associated with an IODE Database which hasn't been saved the last time 
            // the user quitted the IODE GUI (i.e. no filepath associated)
            if (filepath.startsWith(prefixUnsavedDatabase))
            {
                // should never happen
                if(filetype > FILE_VARIABLES)
                {
                    QMessageBox::warning(nullptr, "WARNING", QString("Cannot create a tab related to a ") + 
                        "previously unsaved IODE database.\nGot filetype " + QString::number(filetype) + 
                        " which is higher than " + QString::number(FILE_VARIABLES) + ".");
                    index = -1;
                }
                else
                {
                    index = updateObjectTab((IodeType) filetype);
                    if(index != i) 
                        this->tabBar()->moveTab(index, i);
                }
            }
            // Tab associated with a file
            else 
                index = loadFile(filepath, false, true, i, forcedAsText);

            if(index < 0)
                continue;

            // split tab if necessary
            if(splitted)
                splitTab(index, splitOrientation); 

            IodeAbstractWidget* tabWidget = static_cast<IodeAbstractWidget*>(this->widget(index));
            if(tabWidget)
                tabWidget->loadSettings(project_settings);
        }
        progress.setValue(size);
    }
    project_settings->endArray();

    buildFilesList();

    // get the position of the tab displayed the last time the user quitted the IODE GUI
    index = project_settings->value("index_last_open_tab", "-1").toInt();
    
    project_settings->endGroup();
    
    // display the tab that was displayed the last time the user quitted the IODE GUI
    showTab(index);
}
 
void IodeTabWidget::saveSettings()
{
    QSettings* project_settings = ProjectSettings::getProjectSettings();
    if(!project_settings)
        return;

    // ---- SAVE SETTINGS ----

    QString filepath;
    IodeFileType filetype;
    IodeAbstractWidget* tabWidget;
    ReportWidget* reportWidget;

    project_settings->beginGroup("PROJECT");

    // clear the "tabs" entry
    project_settings->remove("tabs");

    // save the list of open tabs
    project_settings->beginWriteArray("tabs");
    for (int i=0; i < this->count(); i++)
    {
        project_settings->setArrayIndex(i);
        tabWidget = static_cast<IodeAbstractWidget*>(this->widget(i));

        filetype = tabWidget->getFiletype();
        if(filetype <= FILE_VARIABLES && static_cast<AbstractIodeObjectWidget*>(tabWidget)->isUnsavedDatabase())
            filepath = prefixUnsavedDatabase + " " + QString::fromStdString(v_file_types[filetype].v_ext[0]);
        else
            filepath = tabWidget->getFilepath();

        project_settings->setValue("filepath", filepath);
        project_settings->setValue("filetype", filetype);
        project_settings->setValue("forcedAsText", tabWidget->forcedAsText());
        project_settings->setValue("splitted", tabWidget->splitted());
        project_settings->setValue("splitOrientation", tabWidget->getSplitOrientation());

        if(tabWidget)
            tabWidget->saveSettings(project_settings);
    }
    project_settings->endArray();

    // save index of the currently displayed tab
    QVariant index = QVariant(this->currentIndex());
    project_settings->setValue("index_last_open_tab", index);

    project_settings->endGroup();
}

void IodeTabWidget::setup()
{
    MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
    
    // set completer
    this->completer = main_window->getCompleter();

    // set output
    this->output = main_window->getOutput();

    // connects to appendDialog() slot
	connect(this, &IodeTabWidget::newObjsListDialog, main_window, &MainWindowAbstract::appendDialog);

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
        IodeAbstractWidget* tabWidget = static_cast<IodeAbstractWidget*>(this->widget(index));
        setTabText(index, tabWidget->getTabText());
        setTabToolTip(index, tabWidget->getTooltip());
    }

    loadSettings();
    this->show();
}

void IodeTabWidget::resetFilters()
{
    foreach(AbstractIodeObjectWidget* tabWidget, tabIodeObjects) tabWidget->resetFilter();
}

void IodeTabWidget::resetFilter(const IodeType iodeType)
{
    AbstractIodeObjectWidget* tabWidget = getIodeObjTab(iodeType);
    if(tabWidget)
        tabWidget->resetFilter();
}

void IodeTabWidget::clearWorkspace()
{
    foreach(AbstractIodeObjectWidget* tabWidget, tabIodeObjects) tabWidget->clearKDB();
    for(int i=0; i < IODE_NB_TYPES; i++) updateObjectTab((IodeType) i);
}

QStringList IodeTabWidget::getSelectedObjectsNames(const IodeType iodeType)
{
        AbstractIodeObjectWidget* tabWidget = getIodeObjTab(iodeType);
        if(tabWidget)
            return tabWidget->getSelectedObjectsNames();
        else
            return QStringList();
}

int IodeTabWidget::updateObjectTab(const IodeType iodeType)
{
    int index;
    try
    {
        index = getIodeObjTabIndex(iodeType);
        if(index < 0)
            return index;

        // extract corresponding tab widget
        AbstractIodeObjectWidget* tabWidget = static_cast<AbstractIodeObjectWidget*>(this->widget(index));

        // compute hash
        tabWidget->computeHash();

        // try to rerun filter (with silent = true)
        tabWidget->filter(true);

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

int IodeTabWidget::loadFile(const QString& filepath, const bool displayTab, 
    bool forceOverwrite, const int moveToPosition, const bool forceAsText)
{
    QString filename;
    QString tooltip;
    
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

    IodeFileType filetype = get_iode_file_type(fullPath.toStdString());

    // checks if the file has been already loaded
    // Note: indexOf() returns -1 if not found
    buildFilesList();
    int index = filesList.indexOf(fullPath);
    if(index >= 0)
    {
        QMessageBox::StandardButton reloadFile = QMessageBox::No;
        if(filetype <= FILE_VARIABLES)
        {
            reloadFile = QMessageBox::question(nullptr, "WARNING", "The IODE database " + fileInfo.fileName() + 
                " is already open.\nWould you like to reload it?");
        }

        if(reloadFile == QMessageBox::Yes)
            forceOverwrite = true;
        else
        {
            // if file is open in a tab, shows the tab
            this->setCurrentIndex(index);
            return index;
        }
    }

    // load file
    try
    {
        // load KDB file
        if(filetype <= FILE_VARIABLES)
        {
            AbstractIodeObjectWidget* tabWidget = tabIodeObjects[filetype];
            bool success = tabWidget->load(fullPath, forceOverwrite);
            if(success)
            {
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
            index = addNewTab(filetype, fileInfo, forceAsText);

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

void IodeTabWidget::openAddDialog(const IodeType iodeType)
{
    int index = getIodeObjTabIndex(iodeType);
    AbstractIodeObjectWidget* tabWidget = static_cast<AbstractIodeObjectWidget*>(this->widget(index));
    tabWidget->openAddDialog();
}

void IodeTabWidget::openEditDialog(const IodeType iodeType)
{
    int index = getIodeObjTabIndex(iodeType);
    AbstractIodeObjectWidget* tabWidget = static_cast<AbstractIodeObjectWidget*>(this->widget(index));
    tabWidget->openEditDialog();
}

void IodeTabWidget::clearTab()
{
    int index = (indexContextMenu > 0) ? indexContextMenu : currentIndex();
    IodeAbstractWidget* tabWidget = static_cast<IodeAbstractWidget*>(this->widget(index));
    IodeFileType filetype = tabWidget->getFiletype();
    
    if (filetype <= FILE_VARIABLES)
    {
        QMessageBox::StandardButton answer = QMessageBox::warning(nullptr, "WARNING", 
            "Are you sure to clear the " + QString::fromStdString(v_file_types[filetype].v_ext[0]) + " database", 
            QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
        if (answer == QMessageBox::Yes)
        {
            AbstractIodeObjectWidget* kdbWidget = static_cast<AbstractIodeObjectWidget*>(tabWidget);
            kdbWidget->clearKDB();
            updateObjectTab((IodeType) filetype);
        }
    }

    indexContextMenu = -1;
}

void IodeTabWidget::showObjectsList(IodeType iodeType, const QStringList& objNames)
{
    int nb_objs = objNames.size();
    if(nb_objs == 0)
        return;

    QDialog* dialog = new QDialog(nullptr);

    try
    {
        QGridLayout* gridLayout = new QGridLayout(dialog);
        gridLayout->setObjectName("gridLayout");

        AbstractIodeObjectWidget* databaseWidget;
        switch (iodeType)
        {
        case COMMENTS:
            databaseWidget = new CommentsWidget(dialog);
            break;
        case EQUATIONS:
            databaseWidget = new EquationsWidget(dialog);
            break;
        case IDENTITIES:
            databaseWidget = new IdentitiesWidget(dialog);
            break;
        case LISTS:
            databaseWidget = new ListsWidget(dialog);
            break;
        case SCALARS:
            databaseWidget = new ScalarsWidget(dialog);
            break;
        case TABLES:
            databaseWidget = new TablesWidget(dialog);
            break;
        case VARIABLES:
            databaseWidget = new VariablesWidget(dialog);
            break;
        default:
            break;
        }

        // The user can modify the values of the objects
        // Informs the tab representing the IODE objects of type 'iodeType' that a value has been changed 
        int index = getIodeObjTabIndex(iodeType);
        if(index < 0)
        {
            std::string type_ = v_iode_types[iodeType];
            throw IodeException("Cannot show IODE objects of type " + type_ + "\n" +
                + "Database of objects of type " + type_ + " not found");
        }
        IodeAbstractWidget* tab = static_cast<IodeAbstractWidget*>(this->widget(index));
        connect(databaseWidget, &AbstractIodeObjectWidget::tabDatabaseModified, tab, 
            [tab](const IodeType iodeType, const bool modified){ tab->setModified(modified); });

        // update the view -> computes the columns names if variables
        databaseWidget->update();

        // filter names
        QString pattern = objNames.join(";");
        databaseWidget->filter(pattern);

        // adds the IODE objects table view to the popup dialog box
        databaseWidget->setObjectName("databaseWidget");
        gridLayout->addWidget(databaseWidget, 0, 0, 1, 1);

        // makes the dialog box non modal
        dialog->setWindowModality(Qt::NonModal);
        dialog->setModal(false);

        // resize the dialog window
        int h = qMin(30 + databaseWidget->preferredHeight(), this->height());
        dialog->resize(this->width(), h);

        // appends the new dialog box the list of open dialog boxes in the Main Window
        emit newObjsListDialog(dialog);
    }
    catch(const std::exception& e)
    {
        delete dialog;
        QMessageBox::critical(nullptr, "ERROR", QString(e.what()));
    }
}