#include "main_window.h"

QWidget* main_window_ptr = nullptr;


QWidget* get_main_window_ptr()
{
    return main_window_ptr;
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), project_settings(nullptr)
{
    // ---- setup IODE ----
    IodeInit();

    // ---- setup the present class ----
    setupUi(this);

    // ---- global parameters ----
    main_window_ptr = static_cast<QWidget*>(this);

    // ---- user settings ----
    user_settings = new QSettings(QSettings::UserScope, this);
    rootPath = user_settings->value("rootPath", QDir::currentPath()).toString();
    recentProjects = user_settings->value("recentProjects", QStringList()).toStringList();

    // ---- menus ----
    buildRecentProjectsMenu();

    // ---- file explorer ----
    treeView_file_explorer->setIodeTabWidget(tabWidget_IODE_objs);

    // ---- load project ----
    openDirectory(rootPath);
}

MainWindow::~MainWindow()
{
    user_settings->setValue("rootPath", QVariant(rootPath));

    delete project_settings;
    delete user_settings;

    IodeEnd();
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

void MainWindow::openDirectory(const QString& dirPath)
{
    if(dirPath.isEmpty())
    {
        QMessageBox::critical(this, "Error", "Empty path for new Project");
        return;
    }

    QDir dir(dirPath);
    rootPath = dir.absolutePath();
    if (!dir.exists())
    {
        QMessageBox::critical(this, "Error", "Directory " + rootPath + " does not exist");
        return;
    }

    // create/update settings
    QDir projectDir(dirPath);
    QString qSettingsFilepath = projectDir.absoluteFilePath("iode_gui_settings.ini");
    project_settings_filepath = std::make_shared<QString>(qSettingsFilepath);

    if (project_settings) delete project_settings;
    project_settings = new QSettings(qSettingsFilepath, QSettings::IniFormat);

    // close all tabs
    tabWidget_IODE_objs->clear();

    // update file explorer view
    treeView_file_explorer->updateProjectDir(projectDir, project_settings_filepath);

    // (re)open tabs
    tabWidget_IODE_objs->setup(project_settings_filepath);

    // add directory path to list of recently opened projects (= directories)
    addProjectPathToList(projectDir);
}

void MainWindow::open_project()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), rootPath,
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    // check if user clikced on Cancel button
    if(dir.isEmpty()) return;
    // update current project path + open it in the Iode GUI File Exporer
    rootPath = dir;
    openDirectory(rootPath);
}

void MainWindow::save_project_as()
{
    // QFileDialog::getExistingDirectory allows users to create a new directory while the file explorer is open
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), rootPath,
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    // check if user clikced on Cancel button
    if(dir.isEmpty()) return;
    // update current project path
    rootPath = dir;
    // save all Iode files to the (new) directory 
    QDir projectDir = QDir(rootPath);
    tabWidget_IODE_objs->saveProjectAs(projectDir);

    openDirectory(rootPath);
}

void MainWindow::open_recent_project()
{
    QAction* action = static_cast<QAction*>(QObject::sender());
    if (action)
    {
        QString projectPath = action->data().toString();
        if (QDir(projectPath).exists()) openDirectory(projectPath);
        else 
        {
            QMessageBox::warning(this, "Warning", "Directory " + projectPath + " seems to no longer exist");
            recentProjects.removeAll(projectPath);
            user_settings->setValue("recentProjects", QVariant::fromValue(recentProjects));
        }
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    event->accept();
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
        this->lineEdit_filter_comments->setText("");
        this->tableview_comments->filter();
    }
}

void MainWindow::open_import_variables_dialog()
{
    QIodeMenuFileImportVariables dialog(*project_settings_filepath, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        this->lineEdit_filter_variables->setText("");
        this->tableview_variables->filter();
    }
}

void MainWindow::open_export_dialog()
{
    QIodeMenuFileExport dialog(*project_settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_load_workspace_dialog()
{
    QIodeMenuWorkspaceLoad dialog(*project_settings_filepath, tabWidget_IODE_objs, this);
    dialog.exec();
    tabWidget_IODE_objs->resetFilters();
}

void MainWindow::open_save_workspace_dialog()
{
    QIodeMenuWorkspaceSave dialog(*project_settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_clear_workspace_dialog()
{
    QIodeMenuWorkspaceClear dialog(*project_settings_filepath, this);
    dialog.exec();
    tabWidget_IODE_objs->resetFilters();
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
    QUrl url = get_url_iode_helpfile("readme.htm");
    QDesktopServices::openUrl(url);
}

void MainWindow::open_iode_home()
{
    QDesktopServices::openUrl(QUrl(IODE_WEBSITE));
}

void MainWindow::open_iode_manual()
{
    QUrl url = get_url_iode_manual();
    QDesktopServices::openUrl(url);
}
