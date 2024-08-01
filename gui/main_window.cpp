#include "main_window.h"

QWidget* main_window_ptr = nullptr;


QWidget* get_main_window_ptr()
{
    return main_window_ptr;
}


MainWindow::MainWindow(QWidget *parent, const QVector<QString>& files_to_load) : MainWindowAbstract(parent)
{
    // ---- setup the present class ----
    setupUi(this);

    // ---- prepare auto-completion ----
    completer = new IodeCompleter(true, false, -1, this);

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

    // ---- open file(s) passed as files_to_load argument ----
    QVector<QString> files_to_load_;
    if(files_to_load.size() > 0)
    {
        QString parent_dir;
        for(const QString& filepath : files_to_load)
        {
            QFileInfo fileInfo(filepath);
            if (!fileInfo.exists())
                QMessageBox::warning(this, "WARNING", "Cannot open '" + filepath + "'. File does not exist");
            
            if (!fileInfo.isFile())
            {
                    QMessageBox::warning(this, "WARNING", "Only files can be open");
                    break;
            }
            else
            {
                if(parent_dir.isEmpty())
                    parent_dir = fileInfo.absolutePath();
                if(fileInfo.absolutePath() != parent_dir)
                {
                    QMessageBox::warning(this, "WARNING", "All files to open must come from the same directory");
                    parent_dir = "";
                    files_to_load_.clear();
                    break;
                }
                files_to_load_.append(fileInfo.absoluteFilePath());
            }
        }

        if(!parent_dir.isEmpty())
            projectPath = parent_dir;
    }

    // ---- menus ----
    buildRecentProjectsMenu();

    // ---- file explorer ----
    // connect the Tabs widget to the File Explorer
    treeView_file_explorer->setIodeTabWidget(tabWidget_IODE_objs);

    // ---- iode commands line ----
    // setup the line widget to execute IODE commands
    lineEdit_iode_command->setup();

    // ---- shortcuts ----
    fullScreenShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_X), this);
    fullScreenShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);

    // ---- signals and slots ----
    connect(lineEdit_iode_command, &IodeCommandLine::askComputeHash, tabWidget_IODE_objs, &IodeTabWidget::computeHash);

    connect(fullScreenShortcut, &QShortcut::activated, this, &MainWindow::showMaximized);

    // ---- load project (if any) (if any) ----
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

    // files_to_load is not empty
    if(files_to_load_.size() > 0)
    {
        for(const QString& filepath: files_to_load_)
            tabWidget_IODE_objs->loadFile(filepath, true, true);
    }
}

MainWindow::~MainWindow()
{
    user_settings->setValue("projectPath", QVariant(projectPath));
    user_settings->setValue("fontFamily", QVariant(fontFamily));

    delete user_settings;

    delete completer;

    delete fullScreenShortcut;
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
    QMessageBox::StandardButton answer = QMessageBox::warning(this, "WARNING", "Save content of all open tabs?", 
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Discard, QMessageBox::Yes);
    if (answer == QMessageBox::Yes) tabWidget_IODE_objs->saveAllTabs();
    return answer;
}

bool MainWindow::openDirectory(const QString& dirPath)
{
    if(dirPath.isEmpty())
    {
        QMessageBox::warning(this, "WARNING", "Empty path for new Project");
        return false;
    }

    QDir dir(dirPath);
    projectPath = dir.absolutePath();
    if (!dir.exists())
    {
        QMessageBox::warning(this, "WARNING", "Directory " + projectPath + " does not exist");
        return false;
    }

    // create/update settings
    treeView_file_explorer->saveSettings();
    tabWidget_IODE_objs->saveSettings();
    QDir projectDir(dirPath);
    ProjectSettings::changeProject(projectDir, this);

    // update current directory (chdir)
    QDir::setCurrent(projectDir.absolutePath());

    // show File Explorer widget (in case it was hidden)
    dockWidget_file_explorer->show();

    // update file explorer view
    treeView_file_explorer->updateProjectDir(projectDir);

    // (re)open tabs
    tabWidget_IODE_objs->setup();

    // add directory path to list of recently opened projects (= directories)
    addProjectPathToList(projectDir);

    // set currentProjectPath global settings
    user_settings->setValue("projectPath", QVariant(projectPath));

    // update auto-completion
    completer->updateIodeOjectsListNames();

    return true;
}

// -------- File Menu --------

void MainWindow::createNewProject()
{
    if (!projectPath.isEmpty())
    {
        // ask to save all current tabs content before to switch
        QMessageBox::StandardButton answer = askSaveAllTabs();
        if (answer == QMessageBox::Discard) return;
    }

    MenuFileNewProject dialog(projectPath, this);
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
            QMessageBox::warning(this, "WARNING", "Directory " + projectPath + " seems to no longer exist");
            recentProjects.removeAll(projectPath);
            user_settings->setValue("recentProjects", QVariant::fromValue(recentProjects));
        }
    }
}

void MainWindow::saveCurrentTab()
{
    tabWidget_IODE_objs->saveTab();
}

void MainWindow::saveAllTabs()
{
    tabWidget_IODE_objs->saveAllTabs();
}

// NOTE: In case users ask to restore Alt + F<...> behavior.
//       Alt + F4 is the OS (Windows) shortcut to close a program.
//       So, Alt + F4 will actually triggers the closeEvent() method and since it is
//       triggered directly by the OS, it will bypass the keyPressEvent() method.
//       To force closeEvent() not to proceed and to propagate the key combination Alt + F4 
//       to the whole program, you can do the following:
//
//       void MainWindow::closeEvent(QCloseEvent* event)
//       {
//           // Check if the close event has been triggered by Alt or any Alt + <...> key combination
//           // In that case, create a QKeyEvent CTRL + F4 and post it in the events queue
//           if ((QApplication::queryKeyboardModifiers() == Qt::AltModifier))
//           {
//               event->ignore();
//               QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_F4, Qt::AltModifier);
//               // postEvent() posts the event on a queue for later dispatch. 
//               // The next time Qt's main event loop runs, it dispatches all posted events, with some optimization. 
//               // https://doc.qt.io/qt-6/eventsandfilters.html#sending-events
//               QApplication::postEvent(this, event);
//               return;
//           }
//           (...)
//        }

void MainWindow::closeEvent(QCloseEvent* event)
{
    // ask to save all current tabs content before to switch
    QMessageBox::StandardButton answer = askSaveAllTabs();
    if (answer == QMessageBox::Discard) 
        event->ignore();
    else
    {
        foreach(QDialog* dialog, dialogs) dialog->close();
        dialogs.clear();
        
        event->accept();
    }
}

void MainWindow::check_vars_sample()
{
    // check variables sample and ask to set it if not already defined
	if (Variables.get_nb_periods() == 0)
	{
		QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "Sample", "Sample undefined. Set it?");
		if (reply == QMessageBox::Yes)
		{
            computeHash(true);

            try
            {
                EditIodeSampleDialog dialog(this);
                if (dialog.exec() == QDialog::Accepted)
                {
                    std::string from = dialog.get_from().toStdString();
                    std::string to = dialog.get_to().toStdString();
                    Variables.set_sample(from, to);
                }
                
                update_tab_and_completer(VARIABLES);
            }
            catch(const std::exception& e)
            {
                QMessageBox::warning(nullptr, tr("WARNING"), tr(e.what()));
            }
		}
	}
}

void MainWindow::open_import_comments_dialog()
{
    computeHash(true);

    MenuFileImportComments dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        tabWidget_IODE_objs->resetFilter(COMMENTS);
        update_tab_and_completer(COMMENTS);
    }
}

void MainWindow::open_import_variables_dialog()
{
    computeHash(true);

    MenuFileImportVariables dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        tabWidget_IODE_objs->resetFilter(VARIABLES);
        update_tab_and_completer(VARIABLES);
    }
}

void MainWindow::open_export_dialog()
{
    MenuFileExport dialog(this);
    dialog.exec();
}

void MainWindow::open_settings()
{
    MenuFileSettings dialog(this);
    dialog.exec();
}

// -------- Workspace Menu --------

void MainWindow::clear_workspace()
{
    QMessageBox::StandardButton answer = QMessageBox::warning(this, "WARNING", "Are you sure to clear the whole workspace?", 
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::Yes)
    {
        computeHash(true);
        tabWidget_IODE_objs->clearWorkspace();
        update_tab_and_completer();
    }
}

void MainWindow::open_copy_into_workspace_dialog()
{
    computeHash(true);
    MenuWorkspaceCopyInto dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        tabWidget_IODE_objs->resetFilters();
        update_tab_and_completer();
    }
}

void MainWindow::open_merge_into_workspace_dialog()
{
    computeHash(true);
    MenuWorkspaceMergeInto dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        tabWidget_IODE_objs->resetFilters();
        update_tab_and_completer();
    }
}

void MainWindow::open_change_workspace_descriptions_dialog()
{
    MenuWorkspaceDescribe dialog(this);
    dialog.exec();
}

void MainWindow::open_change_variables_sample_dialog()
{
    computeHash(true);

    try
    {
        EditIodeSampleDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted)
        {
            std::string from = dialog.get_from().toStdString();
            std::string to = dialog.get_to().toStdString();
            Variables.set_sample(from, to);
        }
        
        update_tab_and_completer(VARIABLES);
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, tr("WARNING"), tr(e.what()));
    }
}

void MainWindow::open_extrapolate_variables_dialog()
{
    computeHash(true);

    MenuWorkspaceExtrapolateVariables dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        tabWidget_IODE_objs->resetFilter(VARIABLES);
        update_tab_and_completer(VARIABLES);
    }
}

void MainWindow::open_high_to_low_dialog()
{
    check_vars_sample();
    computeHash(true);

    MenuWorkspaceHighToLow dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        tabWidget_IODE_objs->resetFilter(VARIABLES);
        update_tab_and_completer(VARIABLES);
    }
}

void MainWindow::open_low_to_high_dialog()
{
    check_vars_sample();
    computeHash(true);
    
    MenuWorkspaceLowToHigh dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        tabWidget_IODE_objs->resetFilter(VARIABLES);
        update_tab_and_completer(VARIABLES);
    }
}

void MainWindow::open_seasonal_adjustment_dialog()
{
    computeHash(true);

    MenuWorkspaceSeasonalAdjustment dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer();
}

void MainWindow::open_trend_correction_dialog()
{
    computeHash(true);

    MenuWorkspaceTrendCorrection dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer();
}

// -------- Data Menu --------

void MainWindow::open_sort_and_calculus_dialog()
{
    computeHash(true);

    MenuDataSortExpandList dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer(LISTS);
}

void MainWindow::open_list_calculus_dialog()
{
    computeHash(true);

    MenuDataListCalculus dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer(LISTS);
}

void MainWindow::open_file_contents_dialog()
{
    computeHash(true);

    MenuDataFileContents dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer(LISTS);
}

void MainWindow::open_file_compare_dialog()
{
    computeHash(true);

    MenuDataFileCompare dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer(LISTS);
}

void MainWindow::open_search_text_dialog()
{
    computeHash(true);

    MenuDataSearchText dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer(LISTS);
}

void MainWindow::open_scan_objects_dialog()
{
    computeHash(true);

    MenuDataScanObjects dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer(LISTS);
}

void MainWindow::open_edit_tables_variables_dialog()
{
    computeHash(true);

    MenuDataEditTables dialog(this);
    dialog.exec();
}

void MainWindow::open_duplicate_objects_dialog()
{
    computeHash(true);

    MenuDataDuplicateObj dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer();
}

// -------- Run Menu --------

void MainWindow::open_compute_identities_dialog()
{
    computeHash(true);

    MenuComputeIdentities dialog(this);

    QString identites;
    QStringList identitiesList = tabWidget_IODE_objs->getSelectedObjectsNames(IDENTITIES);
    if(identitiesList.size() > 0)
        dialog.setIdentitiesNames(identitiesList);

    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer();
}

void MainWindow::open_compute_simulation_dialog()
{
    computeHash(true);

    MenuComputeSimulation dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer();
}

void MainWindow::open_compute_model_dialog()
{
    computeHash(true);

    MenuComputeModel dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer();
}

void MainWindow::open_compute_scc_decomposition_dialog()
{
    computeHash(true);

    MenuComputeSCCDecomposition dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer();
}

void MainWindow::open_compute_scc_simulation_dialog()
{
    computeHash(true);

    MenuComputeSCCSimulation dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        update_tab_and_completer();
}
// -------- Print/Graphs Menu --------

void MainWindow::open_print_tables_dialog()
{
    MenuPrintTables dialog(this);
    dialog.exec();
}

void MainWindow::open_print_variables_dialog()
{
    MenuPrintVariables dialog(this);
    dialog.exec();
}

void MainWindow::open_print_objects_definitions_dialog()
{
    QMessageBox::warning(this, "WARNING", "Object Definitions not yet impemented");
}

void MainWindow::open_graphs_tables_dialog()
{
    MenuGraphTables dialog(this);
    connect(&dialog, &MenuGraphTables::newPlot, this, &MainWindow::appendDialog);
    dialog.exec();
}

void MainWindow::open_graphs_variables_dialog()
{
    MenuGraphVariables dialog(this);
    connect(&dialog, &MenuGraphVariables::newPlot, this, &MainWindow::appendDialog);
    dialog.exec();
}

void MainWindow::open_graphs_variables_dialog_from_vars_view(
    const QList<QString>& variableNames, const QString& from, const QString& to)
{
    MenuGraphVariables dialog(this);
    dialog.setVariablesNames(variableNames);
	dialog.setFrom(from);
	dialog.setTo(to);
    connect(&dialog, &MenuGraphVariables::newPlot, this, &MainWindow::appendDialog);
    dialog.exec();
}

// -------- Help Menu --------

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
    QDesktopServices::openUrl(url_readme);
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
    QDesktopServices::openUrl(url_shortcuts);
}

// -------- Other slots --------

void MainWindow::update_tab_and_completer(const int iodeType)
{
    int index = -1;

    // compute hash + update tab(s)
    if(iodeType > 0)
        index = tabWidget_IODE_objs->updateObjectTab((IodeDatabaseType) iodeType);
    else
        for(int i = 0; i < IODE_NB_TYPES; i++)
            tabWidget_IODE_objs->updateObjectTab((IodeDatabaseType) i);

        // update the list of Iode object names available for autocomplete
        completer->updateIodeOjectsListNames();
        // show corresponding tab
        tabWidget_IODE_objs->showTab(index);
}
