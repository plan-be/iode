#include "main_window.h"

QWidget* main_window_ptr = nullptr;


QWidget* get_main_window_ptr()
{
    return main_window_ptr;
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // ---- setup IODE ----
    IodeInit();

    // ---- setup the present class ----
    setupUi(this);

    // ---- global parameters ----
    main_window_ptr = static_cast<QWidget*>(this);

    // ---- settings ----
    settings_filepath = std::make_shared<QString>("iode_gui_settings.ini");
    settings = new QSettings(*settings_filepath, QSettings::IniFormat);

    // ---- tabs ----
    tabWidget_IODE_objs->setup(settings_filepath);
}

MainWindow::~MainWindow()
{
    delete settings;

    IodeEnd();
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
    QIodeMenuWorkspaceLoad dialog(*settings_filepath, tabWidget_IODE_objs, this);
    dialog.exec();
    tabWidget_IODE_objs->resetFilters();
}

void MainWindow::open_save_workspace_dialog()
{
    QIodeMenuWorkspaceSave dialog(*settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_clear_workspace_dialog()
{
    QIodeMenuWorkspaceClear dialog(*settings_filepath, this);
    dialog.exec();
    tabWidget_IODE_objs->resetFilters();
}

void MainWindow::open_high_to_low_dialog()
{
    check_vars_sample();
    QIodeMenuWorkspaceHighToLow dialog(*settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_low_to_high_dialog()
{
    check_vars_sample();
    QIodeMenuWorkspaceLowToHigh dialog(*settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_compute_identities_dialog()
{
    QIodeMenuComputeIdentities dialog(*settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_compute_simulation_dialog()
{
    QIodeMenuComputeSimulation dialog(*settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_compute_model_dialog()
{
    QIodeMenuComputeModel dialog(*settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_compute_scc_decomposition_dialog()
{
    QIodeMenuComputeSCCDecomposition dialog(*settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_compute_scc_simulation_dialog()
{
    QIodeMenuComputeSCCSimulation dialog(*settings_filepath, this);
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
