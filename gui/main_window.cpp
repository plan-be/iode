#include "main_window.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	setupUi(this);

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
}

void MainWindow::updateCurrentTab(int index)
{
    // get the index of the tab currently visible if not passed to the method
    if (index < 0) index = this->tabWidget_IODE_objs->currentIndex();

    // update the corresponding model and view
    switch (index)
    {
    case COMMENTS:
        this->tableview_comments->update();
        break;
    case EQUATIONS:
        this->tableview_equations->update();
        break;
    case IDENTITIES:
        this->tableview_identities->update();
        break;
    case LISTS:
        this->tableview_lists->update();
        break;
    case SCALARS:
        this->tableview_scalars->update();
        break;
    case TABLES:
        this->tableview_tables->update();
        break;
    case VARIABLES:
        this->tableview_variables->update();
        break;
    default:
        break;
    }
}
