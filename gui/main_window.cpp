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

    // ---- connect signals to slots  ----
    connect(this->tabWidget_IODE_objs, &QTabWidget::currentChanged, this, &MainWindow::viewTab);

    // ---- Model/View components ----

    QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";

    // Comments
    commentsModel = new CommentsModel(this);
    this->tableview_comments->setupView(commentsModel, this->lineEdit_filter_comments, settings_filepath);
    this->tableview_comments->setStyleSheet(stylesheet);
    this->tableview_comments->hide();
    // Equations
    equationsModel = new EquationsModel(this);
    this->tableview_equations->setupView(equationsModel, this->lineEdit_filter_equations, settings_filepath);
    this->tableview_equations->setStyleSheet(stylesheet);
    this->tableview_equations->hide();
    // Identities
    identitiesModel = new IdentitiesModel(this);
    this->tableview_identities->setupView(identitiesModel, this->lineEdit_filter_identities, settings_filepath);
    this->tableview_identities->setStyleSheet(stylesheet);
    this->tableview_identities->hide();
    // Lists
    listsModel = new ListsModel(this);
    this->tableview_lists->setupView(listsModel, this->lineEdit_filter_lists, settings_filepath);
    this->tableview_lists->setStyleSheet(stylesheet);
    this->tableview_lists->hide();
    // Scalars
    scalarsModel = new ScalarsModel(this);
    this->tableview_scalars->setupView(scalarsModel, this->lineEdit_filter_scalars, settings_filepath);
    this->tableview_scalars->setStyleSheet(stylesheet);
    this->tableview_scalars->hide();
    // Tables
    tablesModel = new TablesModel(this);
    this->tableview_tables->setupView(tablesModel, this->lineEdit_filter_tables, settings_filepath);
    this->tableview_tables->setStyleSheet(stylesheet);
    this->tableview_tables->hide();
    // Variables
    variablesModel = new VariablesModel(this);
    this->tableview_variables->setupView(variablesModel, this->lineEdit_filter_variables, settings_filepath);
    this->tableview_variables->setStyleSheet(stylesheet);
    this->tableview_variables->hide();
}

MainWindow::~MainWindow()
{
    delete commentsModel;
    delete equationsModel;
    delete identitiesModel;
    delete listsModel;
    delete scalarsModel;
    delete tablesModel;
    delete variablesModel;

    IodeEnd();
}

void MainWindow::resetFilter()
{
    // get the index of the tab currently visible if not passed to the method
    int index = this->tabWidget_IODE_objs->currentIndex();

    // update the corresponding model and view
    switch (index)
    {
    case I_COMMENTS:
        this->lineEdit_filter_comments->setText("");
        this->tableview_comments->filter();
        break;
    case I_EQUATIONS:
        this->lineEdit_filter_equations->setText("");
        this->tableview_equations->filter();
        break;
    case I_IDENTITIES:
        this->lineEdit_filter_identities->setText("");
        this->tableview_identities->filter();
        break;
    case I_LISTS:
        this->lineEdit_filter_lists->setText("");
        this->tableview_lists->filter();
        break;
    case I_SCALARS:
        this->lineEdit_filter_scalars->setText("");
        this->tableview_scalars->filter();
        break;
    case I_TABLES:
        this->lineEdit_filter_tables->setText("");
        this->tableview_tables->filter();
        break;
    case I_VARIABLES:
        this->lineEdit_filter_variables->setText("");
        this->tableview_variables->filter();
        break;
    default:
        break;
    }
}

void MainWindow::viewTab(int index)
{
    // get the index of the tab currently visible if not passed to the method
    if (index < 0) index = this->tabWidget_IODE_objs->currentIndex();

    // update the corresponding model and view
    switch (index)
    {
    case I_COMMENTS:
        this->tableview_comments->update();
        break;
    case I_EQUATIONS:
        this->tableview_equations->update();
        break;
    case I_IDENTITIES:
        this->tableview_identities->update();
        break;
    case I_LISTS:
        this->tableview_lists->update();
        break;
    case I_SCALARS:
        this->tableview_scalars->update();
        break;
    case I_TABLES:
        this->tableview_tables->update();
        break;
    case I_VARIABLES:
        this->tableview_variables->update();
        break;
    default:
        break;
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
    QIodeMenuFileImportComments dialog(*settings_filepath, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        this->lineEdit_filter_comments->setText("");
        this->tableview_comments->filter();
    }
}

void MainWindow::open_import_variables_dialog()
{
    QIodeMenuFileImportVariables dialog(*settings_filepath, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        this->lineEdit_filter_variables->setText("");
        this->tableview_variables->filter();
    }
}

void MainWindow::open_export_dialog()
{
    QIodeMenuFileExport dialog(*settings_filepath, this);
    dialog.exec();
}

void MainWindow::open_load_workspace_dialog()
{
    QIodeMenuWorkspaceLoad dialog(*settings_filepath, this);
    dialog.exec();
    resetFilter();
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
    resetFilter();
}

void MainWindow::open_high_to_low_dialog()
{
    check_vars_sample();
    QIodeMenuWorkspaceHighToLow dialog(*settings_filepath, this);
    dialog.exec();
    resetFilter();
}

void MainWindow::open_low_to_high_dialog()
{
    check_vars_sample();
    QIodeMenuWorkspaceLowToHigh dialog(*settings_filepath, this);
    dialog.exec();
    resetFilter();
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
