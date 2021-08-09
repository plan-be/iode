#include "main_window.h"

MainWindow::MainWindow() : QMainWindow()
{
    try 
    {
        // ---- setup SCR4 ----
        init_iode_api();

        // ---- setup the present class ----
        window = new Ui::MainWindow();
        window->setupUi(this);

        // ---- initialize internal variables which point to objects defined using the Qt Designer (.ui file) ----
        tabs = window->tabWidget_IODE_objs;
        connect(tabs, &QTabWidget::currentChanged, this, &MainWindow::updateCurrentTab);

        // ---- Model/View components ----
        
        // Comments
        commentsView = window->tableview_comments;
        commentsModel = new CommentsModel(this);
        commentsView->setupModel(commentsModel);
        commentsView->hide();
        // Equations
        equationsView = window->tableview_equations;
        equationsModel = new EquationsModel(this);
        equationsView->setupModel(equationsModel);
        equationsView->hide();
        // Identities
        identitiesView = window->tableview_identities;
        identitiesModel = new IdentitiesModel(this);
        identitiesView->setupModel(identitiesModel);
        identitiesView->hide();
        // Lists
        listsView = window->tableview_lists;
        listsModel = new ListsModel(this);
        listsView->setupModel(listsModel);
        listsView->hide();         
        // Scalars
        scalarsView = window->tableview_scalars;
        scalarsModel = new ScalarsModel(this);
        scalarsView->setupModel(scalarsModel);
        scalarsView->hide();   
        // Tables
        tablesView = window->tableview_tables;
        tablesModel = new TablesModel(this);
        tablesView->setupModel(tablesModel);
        tablesView->hide();        
        // Variables
        variablesView = window->tableview_variables;
        variablesModel = new VariablesModel(this);
        variablesView->setupModel(variablesModel);
        variablesView->hide();

        // ---- Settings ----
        
        // save the settings in a specific file located in the current directory
        settings = get_local_settings_instance();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        this->close();
    }
}

MainWindow::~MainWindow()
{
    delete settings;

    delete commentsView;
    delete equationsView;
    delete identitiesView;
    delete listsView;
    delete scalarsView;
    delete tablesView;
    delete variablesView;

    delete commentsModel;
    delete equationsModel;
    delete identitiesModel;
    delete listsModel;
    delete scalarsModel;
    delete tablesModel;
    delete variablesModel;

    delete window;
}


// TODO: the User Interface implementation should NOT be aware of how the base API (iode.lib) is implemented.
//       Please provide a higher level base API.
void MainWindow::init_iode_api()
{

    SW_MIN_MEM = 120 * 1024L;
    SW_MIN_SEGS = 2;
    // initializes SCR4
    if (SW_init(1)) throw std::runtime_error("An error occured when called the internal function SW_init()");
    
    // initializes Dynamic Data Exchange (DDE)
    //IodeStartDde();
    // initializes Workspace 
    K_init_ws(0);
}

// TODO: the User Interface implementation should NOT be aware of how the base API (iode.lib) is implemented.
//       Please provide a higher level base API.
void MainWindow::end_iode_api()
{
    // free Workspace
    K_end_ws(0);
    // free SCR4
    SW_end();
    // ???
    W_close();
    // stops Dynamic Data Exchange (DDE)
    //IodeEndDde();
}

void MainWindow::updateCurrentTab(int index)
{
    // get the index of the tab currently visible if not passed to the method
    if (index < 0) index = tabs->currentIndex();

    // update the corresponding model and view
    switch (index)
    {
    case COMMENTS:
        commentsView->update();
        break;
    case EQUATIONS:
        equationsView->update();
    case IDENTITIES:
        identitiesView->update();
    case LISTS:
        listsView->update();
    case SCALARS:
        scalarsView->update();
    case TABLES:
        tablesView->update();
    case VARIABLES:
        variablesView->update();;
    default:
        break;
    }
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    end_iode_api();
    event->accept();
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

void MainWindow::open_import_file_dialog()
{
    FileImportDialog dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_export_file_dialog()
{
    FileExportDialog dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_load_settings_dialog()
{
    LoadSettingsDialog dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_save_settings_dialog()
{
    SaveSettingsDialog dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_reset_settings_dialog()
{
}

void MainWindow::open_load_workspace_dialog()
{
    LoadWorkspaceDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_save_workspace_dialog()
{
    SaveWorkspaceDialog dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_clear_workspace_dialog()
{
    ClearWorkspaceDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_copy_into_workspace_dialog()
{
    CopyIntoWorkspaceDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_merge_into_workspace_dialog()
{
    MergeIntoWorkspaceDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_change_workspace_descriptions_dialog()
{
    ChangeWorkspaceDescriptionsDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_change_variables_sample_dialog()
{
    ChangeVariablesSampleDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_extrapolate_variables_dialog()
{
    ExtrapolateVariablesDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_high_to_low_dialog()
{
    HighToLowDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_low_to_high_dialog()
{
    LowToHighDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_seasonal_adjustment_dialog()
{
    SeasonalAdjustmentDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_trend_correction_dialog()
{
    TrendCorrectionDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_object_editor_dialog()
{
    QObject* sender_obj = sender();
    QString sender_name = sender_obj->objectName();
    QString object_type = sender_name.split('_').last();

    ObjectEditorDialog dialog(*settings, this);
    //ObjectEditorDialog dialog(*settings, this, object_type);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_duplicate_objects_dialog()
{
    DuplicateObjectsDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_compute_simulation_dialog()
{
    ComputeSimulationDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_compute_model_dialog()
{
    ComputeModelDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_compute_scc_decomposition_dialog()
{
    ComputeSccDecompositionDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_compute_scc_simulation_dialog()
{
    ComputeSccSimulationDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_compute_identities_dialog()
{
    ComputIdentitiesDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_print_tables_dialog()
{
    PrintTablesDialog dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_print_variables_dialog()
{
    PrintVariablesDialog dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_print_objects_definitions_dialog()
{
    PrintObjectsDefinitionsDialog dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_graphs_tables_dialog()
{
    PrintGraphsTablesDialog dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_graphs_variables_dialog()
{
    PrintGraphsVariablesDialog dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_print_A2M_file_dialog()
{
    PrintA2MFileDialog dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_print_report_dialog()
{
    PrintReportDialog dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_edit_report_dialog()
{
    ReportEditDialog dialog(*settings, this);
    dialog.exec();
}

void MainWindow::open_execute_report_dialog()
{
    ReportExecuteDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}

void MainWindow::open_execute_report_command_line_dialog()
{
    ReportCommandLineDialog dialog(*settings, this);
    dialog.exec();
    updateCurrentTab();
}
