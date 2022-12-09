#include "main_window.h"

QWidget* main_window_ptr = nullptr;
QString currentProjectPath;


QWidget* get_main_window_ptr()
{
    return main_window_ptr;
}

QString get_current_project_path()
{
    return currentProjectPath;
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), project_settings(nullptr)
{
    // ---- setup the present class ----
    setupUi(this);

    // ---- prepare auto-completion ----
    completer = std::make_shared<QIodeCompleter>(false, this);
    lineEdit_iode_command->setup(textEdit_output, completer);

    // ---- dock widgets ----
    // make left dock widget to extend to bottom of the window
    this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

    // ---- global parameters ----
    main_window_ptr = static_cast<QWidget*>(this);

    // ---- user settings ----
    // location if QSettings::UserScope -> FOLDERID_RoamingAppData
    // see https://doc.qt.io/qt-6/qsettings.html#setPath 
    user_settings = new QSettings(QSettings::UserScope, this);
    projectPath = user_settings->value("projectPath", "").toString();
    recentProjects = user_settings->value("recentProjects", QStringList()).toStringList();
    fontFamily = user_settings->value("fontFamily", defaultFontFamily).toString();

    textEdit_output->setStyleSheet("font-family: " + fontFamily);
    lineEdit_iode_command->setStyleSheet("font-family: " + fontFamily);

    // ---- menus ----
    buildRecentProjectsMenu();

    // ---- file explorer ----
    // connect the Tabs widget to the File Explorer
    treeView_file_explorer->setIodeTabWidget(tabWidget_IODE_objs);

    // ---- tabs widget ----
    VariablesView* variablesView = tabWidget_IODE_objs->getVariablesView();
    connect(variablesView, &VariablesView::newPlot, this, &MainWindow::appendPlot);
    connect(variablesView, &VariablesView::newGraphsDialog, this, 
        &MainWindow::open_graphs_variables_dialog_from_vars_view);
    connect(lineEdit_iode_command, &QIodeCommandLine::askComputeHash, tabWidget_IODE_objs, &QIodeTabWidget::computeHash);

    // ---- load project (if any) (if any) ----
    // first time launching the GUI -> ask the user to either start a new project 
    // or to open an existing folder containing reports and/or KDB files as project
    if (projectPath.isEmpty())
    {
        QMessageBox::information(this, "IODE interface", 
            QString("<p align='center'>First time with the IODE interface ?<br><br>") + 
            "Please gjec the File menu to either create a new project folder or to open an folder containg " + 
            "reports and/or IODE database files.</p>" + 
            "<p align='left'>To open file(s) from the file tree (left panel):<br>" + 
            "- double click on a file,<br>" +
            "- select several files with CTRL and then press ENTER.</p>");
        treeView_file_explorer->hide();
        tabWidget_IODE_objs->hide();
        dockWidget_file_explorer->hide();
    }
    else
    // first time launching the GUI -> ask the user to either start a new project 
    // or to open an existing folder containing reports and/or KDB files as project
    if (projectPath.isEmpty())
    {
        QMessageBox::information(this, "IODE interface", 
            QString("<p align='center'>First time with the IODE interface ?<br><br>") + 
            "Please go the File menu to either create a new project folder or to open an folder containg " + 
            "reports and/or IODE database files.</p>" + 
            "<p align='left'>To open file(s) from the file tree (left panel):<br>" + 
            "- double click on a file,<br>" +
            "- select several files with CTRL and then press ENTER.</p>");
        treeView_file_explorer->hide();
        tabWidget_IODE_objs->hide();
        dockWidget_file_explorer->hide();
    }
    else
        openDirectory(projectPath);
}

MainWindow::~MainWindow()
{
    user_settings->setValue("projectPath", QVariant(projectPath));
    user_settings->setValue("fontFamily", QVariant(fontFamily));

    for(int i=0; i<I_NB_TYPES; i++) clear_global_kdb((EnumIodeType) i);

    foreach(QIodePlotDialog* plotDialog, plots) plotDialog->close();
    plots.clear();

    delete project_settings;
    delete user_settings;
}

void MainWindow::buildRecentProjectsMenu()
{
    menuRecent_Projects->clear();

    QString projectPath;
    for (int i=0; i < recentProjects.count(); i++)
    {
        QDir projectDir(recentProjects.at(i));
        QAction* action = new QAction(projectDir.dirName(), this);
        QString absolutePath = projectDir.absolutePath();
        action->setStatusTip(absolutePath);
        action->setToolTip(absolutePath);
        action->setData(absolutePath);
        action->setVisible(true);
        connect(action, &QAction::triggered, this, &MainWindow::open_recent_project);
        menuRecent_Projects->addAction(action);
    }
}

void MainWindow::appendPlot(QIodePlotDialog* plotDialog)
{
    plotDialog->open();
    plots.append(plotDialog);
}

void MainWindow::addProjectPathToList(const QDir& projectDir)
{
    QString projectPath = projectDir.absolutePath();
    // add project directory path to list of recently opened projects (= directories)

    qsizetype index = recentProjects.indexOf(projectPath);
    if (index == -1)
    {
        if (recentProjects.count() == MAX_RECENT_PROJECTS) recentProjects.pop_back();
        recentProjects.prepend(projectPath);
    }
    else recentProjects.move(index, 0);

    // update user settings
    user_settings->setValue("recentProjects", QVariant::fromValue(recentProjects));

    // setup Menu > Recent Projects > list of recent projects
    buildRecentProjectsMenu();
}

QMessageBox::StandardButton MainWindow::askSaveAllTabs()
{
    QMessageBox::StandardButton answer = QMessageBox::warning(this, "Warning", "Save content of all open tabs?", 
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Discard, QMessageBox::Yes);
    if (answer == QMessageBox::Yes) tabWidget_IODE_objs->saveAllTabs();
    return answer;
}

bool MainWindow::openDirectory(const QString& dirPath)
{
    if(dirPath.isEmpty())
    {
        QMessageBox::critical(this, "Error", "Empty path for new Project");
        return false;
    }

    QDir dir(dirPath);
    projectPath = dir.absolutePath();
    if (!dir.exists())
    {
        QMessageBox::critical(this, "Error", "Directory " + projectPath + " does not exist");
        return false;
    }

    // create/update settings
    QDir projectDir(dirPath);
    QString qSettingsFilepath = projectDir.absoluteFilePath("iode_gui_settings.ini");
    project_settings_filepath = std::make_shared<QString>(qSettingsFilepath);

    if (project_settings) delete project_settings;
    project_settings = new QSettings(qSettingsFilepath, QSettings::IniFormat);

    // update current directory (chdir)
    QDir::setCurrent(projectDir.absolutePath());

    // show File Explorer widget (in case it was hidden)
    dockWidget_file_explorer->show();

    // update file explorer view
    treeView_file_explorer->updateProjectDir(projectDir, project_settings_filepath);

    // (re)open tabs
    tabWidget_IODE_objs->setup(project_settings_filepath, completer, textEdit_output);

    // add directory path to list of recently opened projects (= directories)
    addProjectPathToList(projectDir);

    // set global variable currentProjectPath
    currentProjectPath = projectDir.absolutePath();

    // update auto-completion
    completer->updateIodeOjectsListNames();

    return true;
}

void MainWindow::createNewProject()
{
    if (!projectPath.isEmpty())
    {
        // ask to save all current tabs content before to switch
        QMessageBox::StandardButton answer = askSaveAllTabs();
        if (answer == QMessageBox::Discard) return;
    }

    QIodeMenuFileNewProject dialog(projectPath, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString newProjectPath = dialog.getPathNewProject();
        openDirectory(newProjectPath);
    }
}

void MainWindow::open_project()
{
    if (!projectPath.isEmpty())
    {
        // ask to save all current tabs content before to switch
        QMessageBox::StandardButton answer = askSaveAllTabs();
        if (answer == QMessageBox::Discard) return;
    }

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), projectPath,
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    // check if user clikced on Cancel button
    if(dir.isEmpty()) return;

    // update current project path + open it in the Iode GUI File Exporer
    projectPath = dir;
    openDirectory(projectPath);
}

void MainWindow::open_recent_project()
{
    QAction* action = static_cast<QAction*>(QObject::sender());
    if (action)
    {
        QString projectPath = action->data().toString();
        if (QDir(projectPath).exists())
        {
            // ask to save all current tabs content before to switch
            QMessageBox::StandardButton answer = askSaveAllTabs();
            if (answer == QMessageBox::Discard) return;
            openDirectory(projectPath);
        }
        else 
        {
            QMessageBox::warning(this, "Warning", "Directory " + projectPath + " seems to no longer exist");
            recentProjects.removeAll(projectPath);
            user_settings->setValue("recentProjects", QVariant::fromValue(recentProjects));
        }
    }
}

void MainWindow::saveCurrentTab()
{
    tabWidget_IODE_objs->saveCurrentTab();
}

void MainWindow::saveAllTabs()
{
    tabWidget_IODE_objs->saveAllTabs();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // ask to save all current tabs content before to switch
    QMessageBox::StandardButton answer = askSaveAllTabs();
    if (answer == QMessageBox::Discard) event->ignore();
    else event->accept();
}

void MainWindow::check_vars_sample()
{
    // check variables sample and ask to set it if not already defined
	KDBVariables kdb;
	if (kdb.get_nb_periods() == 0)
	{
		QWidget* p = static_cast<QWidget*>(parent());
		QMessageBox::StandardButton reply = QMessageBox::question(p, "Sample", "Sample undefined. Set it?");
		if (reply == QMessageBox::Yes)
		{
			QIodeEditVarsSample dialog(this);
			dialog.exec();
		}
		else
		{
			return;
		}
	}
}

void MainWindow::open_import_comments_dialog()
{
    QIodeMenuFileImportComments dialog(*project_settings_filepath, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        tabWidget_IODE_objs->resetFilter(I_COMMENTS);
        completer->updateIodeOjectsListNames();
    }
}

void MainWindow::open_import_variables_dialog()
{
    QIodeMenuFileImportVariables dialog(*project_settings_filepath, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        tabWidget_IODE_objs->resetFilter(I_VARIABLES);
        completer->updateIodeOjectsListNames();
    }
}

void MainWindow::open_export_dialog()
{
    QIodeMenuFileExport dialog(*project_settings_filepath, this);
    dialog.exec();
}

void MainWindow::clear_workspace()
{
    QMessageBox::StandardButton answer = QMessageBox::warning(this, "Warning", "Are you sure to clear the whole workspace?", 
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::Yes)
    {
        tabWidget_IODE_objs->clearWorkspace();
        completer->updateIodeOjectsListNames();
    }
}

void MainWindow::open_copy_into_workspace_dialog()
{
    QIodeMenuWorkspaceCopyInto dialog(*project_settings_filepath, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        tabWidget_IODE_objs->resetFilters();
        completer->updateIodeOjectsListNames();
    }
}

void MainWindow::open_merge_into_workspace_dialog()
{
    QIodeMenuWorkspaceMergeInto dialog(*project_settings_filepath, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        tabWidget_IODE_objs->resetFilters();
        completer->updateIodeOjectsListNames();
    }
}

void MainWindow::open_change_workspace_descriptions_dialog()
{
    QIodeMenuWorkspaceDescribe dialog(*project_settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_change_variables_sample_dialog()
{
    QIodeEditVarsSample dialog(this);
    dialog.exec();
}

void MainWindow::open_extrapolate_variables_dialog()
{
    QIodeMenuWorkspaceExtrapolateVariables dialog(*project_settings_filepath, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        tabWidget_IODE_objs->resetFilter(I_VARIABLES);
        // update current tab if needed
        tabWidget_IODE_objs->showTab();
    }
}

void MainWindow::open_high_to_low_dialog()
{
    check_vars_sample();
    QIodeMenuWorkspaceHighToLow dialog(*project_settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_low_to_high_dialog()
{
    check_vars_sample();
    QIodeMenuWorkspaceLowToHigh dialog(*project_settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_compute_identities_dialog()
{
    QIodeMenuComputeIdentities dialog(*project_settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_compute_simulation_dialog()
{
    QIodeMenuComputeSimulation dialog(*project_settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_compute_model_dialog()
{
    QIodeMenuComputeModel dialog(*project_settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_compute_scc_decomposition_dialog()
{
    QIodeMenuComputeSCCDecomposition dialog(*project_settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_compute_scc_simulation_dialog()
{
    QIodeMenuComputeSCCSimulation dialog(*project_settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_graphs_variables_dialog()
{
    QIodeMenuGraphVariables dialog(*project_settings_filepath, this);
    connect(&dialog, &QIodeMenuGraphVariables::newPlot, this, &MainWindow::appendPlot);
    dialog.exec();
}

void MainWindow::open_graphs_variables_dialog_from_vars_view(
    const QList<QString>& variableNames, const QString& from, const QString& to)
{
    QIodeMenuGraphVariables dialog(*project_settings_filepath, this);
    dialog.setVariablesNames(variableNames);
	dialog.setFrom(from);
	dialog.setTo(to);
    connect(&dialog, &QIodeMenuGraphVariables::newPlot, this, &MainWindow::appendPlot);
    dialog.exec();
}

void MainWindow::about()
{
    // TODO : add a IODE_VERSION X.X in api/iode.h (to be more "modern")
    QString version = QString("%1.%2").arg(IODE_VERSION_MAJOR).arg(IODE_VERSION_MINOR);
    QMessageBox::about(this, tr("About IODE"),
        tr(R"(
            <style>
            * {text-align: center;}
            </style>

            <h1>Welcome to IODE</h1>
            <h2>MODELLING SOFTWARE</h2>
            <h2>Version %1</h2>
           
            <h3>Developped by the Federal Planning Bureau (Belgium)</h3>
            <p>
            <ul> 
                <li>Geert Bryon &nbsp;&nbsp;&nbsp;&nbsp;  
                <li>Jean-Marc Paul
                <li>Alix Damman &nbsp;&nbsp;&nbsp;   
            </ul>
            </p>
           
            <h3>Tools</h3>         
            <p>SCR/AL1(c) 1986-2020, JM.B.Paul</p>
           
            <h3>Website</h3>       
            <p><a href='https://iode.plan.be/doku.php'>iode.plan.be</a></p>)").arg(version)
    );
}

void MainWindow::open_release_notes()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("./doc/readme.htm"));
}

void MainWindow::open_iode_home()
{
    QDesktopServices::openUrl(QUrl(IODE_WEBSITE));
}

void MainWindow::open_iode_manual()
{
    QDesktopServices::openUrl(url_manual);
}

void MainWindow::display_keyboard_shortcuts()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("./doc/keyboard_shortcuts.pdf"));
}

void MainWindow::update_current_tab()
{
    // update current tab if needed
    tabWidget_IODE_objs->showTab();
    // update the list of Iode object names available for auto-complemention
    completer->updateIodeOjectsListNames();
}
