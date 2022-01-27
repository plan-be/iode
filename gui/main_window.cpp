#include "main_window.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // ---- setup IODE ----
    IodeInit();

    // ---- setup the present class ----
    setupUi(this);

    // ---- connect signals to slots  ----
    connect(this->tabWidget_IODE_objs, &QTabWidget::currentChanged, this, &MainWindow::updateCurrentTab);

    // ---- Model/View components ----

    // Comments
    commentsModel = new CommentsModel(this);
    this->tableview_comments->setupModel(commentsModel);
    this->tableview_comments->hide();
    // Equations
    equationsModel = new EquationsModel(this);
    this->tableview_equations->setupModel(equationsModel);
    this->tableview_equations->hide();
    // Identities
    identitiesModel = new IdentitiesModel(this);
    this->tableview_identities->setupModel(identitiesModel);
    this->tableview_identities->hide();
    // Lists
    listsModel = new ListsModel(this);
    this->tableview_lists->setupModel(listsModel);
    this->tableview_lists->hide();
    // Scalars
    scalarsModel = new ScalarsModel(this);
    this->tableview_scalars->setupModel(scalarsModel);
    this->tableview_scalars->hide();
    // Tables
    tablesModel = new TablesModel(this);
    this->tableview_tables->setupModel(tablesModel);
    this->tableview_tables->hide();
    // Variables
    variablesModel = new VariablesModel(this);
    this->tableview_variables->setupModel(variablesModel);
    this->tableview_variables->hide();

    // ---- Settings ----

    // save the settings in a specific file located in the current directory
    settings = get_local_settings_instance();
}

MainWindow::~MainWindow()
{
    delete settings;

    delete commentsModel;
    delete equationsModel;
    delete identitiesModel;
    delete listsModel;
    delete scalarsModel;
    delete tablesModel;
    delete variablesModel;

    IodeEnd();
}

void MainWindow::updateCurrentTab(int index)
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

void MainWindow::open_load_workspace_dialog()
{
    QIodeMenuWorkspaceLoad dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_save_workspace_dialog()
{
    QIodeMenuWorkspaceSave dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_clear_workspace_dialog()
{
    QIodeMenuWorkspaceClear dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
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
