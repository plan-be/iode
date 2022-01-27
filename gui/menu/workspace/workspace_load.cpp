#include "workspace_load.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from QIodeSettings MUST:
 * 2. call setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from QIodeSettings) with the wrapped field items,
 * 5. initialize the inherited className member (using typeid(this).name())
 * 6. call the loadSettings() method (inherited from QIodeSettings).
 */


QIodeMenuWorkspaceLoad::QIodeMenuWorkspaceLoad(QSettings& settings, QWidget* parent, Qt::WindowFlags f) : QIodeSettings(settings, parent, f)
{
	setupUi(this);

    mapFields["Comments"] = new WrapperFileChooser(pushButton_comments->text(), *fileChooser_comments, OPTIONAL_FIELD, I_COMMENTS_FILE, EXISTING_FILE);
    mapFields["Equations"] = new WrapperFileChooser(pushButton_equations->text(), *fileChooser_equations, OPTIONAL_FIELD, I_EQUATIONS_FILE, EXISTING_FILE);
    mapFields["Identities"] = new WrapperFileChooser(pushButton_identities->text(), *fileChooser_identities, OPTIONAL_FIELD, I_IDENTITIES_FILE, EXISTING_FILE);
    mapFields["Lists"] = new WrapperFileChooser(pushButton_lists->text(), *fileChooser_lists, OPTIONAL_FIELD, I_LISTS_FILE, EXISTING_FILE);
    mapFields["Scalars"] = new WrapperFileChooser(pushButton_scalars->text(), *fileChooser_scalars, OPTIONAL_FIELD, I_SCALARS_FILE, EXISTING_FILE);
    mapFields["Tables"] = new WrapperFileChooser(pushButton_tables->text(), *fileChooser_tables, OPTIONAL_FIELD, I_TABLES_FILE, EXISTING_FILE);
    mapFields["Variables"] = new WrapperFileChooser(pushButton_variables->text(), *fileChooser_variables, OPTIONAL_FIELD, I_VARIABLES_FILE, EXISTING_FILE);

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

QIodeMenuWorkspaceLoad::~QIodeMenuWorkspaceLoad()
{
}

void QIodeMenuWorkspaceLoad::load_component(KDBAbstract& kdb, const bool accept)
{
    try
    {
        int i_type = kdb.getIODEType();
        std::string s_type = vIodeTypes[i_type];
        QString qs_type = QString::fromStdString(s_type);
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields.value(qs_type));
        QString filepath = field_filepath->extractAndVerify();
        // accept = true means that the users clicked on an individual "Load XXX" button.
        // In that case and if the filepath is empty, we show warning box
        if (accept && filepath.isEmpty())
        {
            QMessageBox::warning(this, tr("WARNING"), QString("Cannot load %1. Filepath is empty.").arg(qs_type));
            return;
        }
        kdb.load(filepath.toStdString());

        if (accept) this->accept();
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}

void QIodeMenuWorkspaceLoad::load_comments()
{
    KDBComments kdb = KDBComments();
    load_component(kdb);
}

void QIodeMenuWorkspaceLoad::load_equations()
{
    KDBEquations kdb = KDBEquations();
    load_component(kdb);
}

void QIodeMenuWorkspaceLoad::load_identities()
{
    KDBIdentities kdb = KDBIdentities();
    load_component(kdb);
}

void QIodeMenuWorkspaceLoad::load_lists()
{
    KDBLists kdb = KDBLists();
    load_component(kdb);
}

void QIodeMenuWorkspaceLoad::load_scalars()
{
    KDBScalars kdb = KDBScalars();
    load_component(kdb);
}

void QIodeMenuWorkspaceLoad::load_tables()
{
    KDBTables kdb = KDBTables();
    load_component(kdb);
}

void QIodeMenuWorkspaceLoad::load_variables()
{
    KDBVariables kdb = KDBVariables();
    load_component(kdb);
}

void QIodeMenuWorkspaceLoad::load()
{
    KDBComments kdb_comments = KDBComments();
    KDBEquations kdb_equations = KDBEquations();
    KDBIdentities kdb_identities = KDBIdentities();
    KDBLists kdb_lists = KDBLists();
    KDBScalars kdb_scalars = KDBScalars();
    KDBTables kdb_tables = KDBTables();
    KDBVariables kdb_variables = KDBVariables();

    load_component(kdb_comments, false);
    load_component(kdb_equations, false);
    load_component(kdb_identities, false);
    load_component(kdb_lists, false);
    load_component(kdb_scalars, false);
    load_component(kdb_tables, false);
    load_component(kdb_variables, false);

    this->accept();
}
