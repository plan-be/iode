#include "workspace_clear.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from QIodeSettings MUST:
 * 2. call setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from QIodeSettings) with the wrapped field items,
 * 5. initialize the inherited className member (using typeid(this).name())
 * 6. call the loadSettings() method (inherited from QIodeSettings).
 */


QIodeMenuWorkspaceClear::QIodeMenuWorkspaceClear(QSettings& settings, QWidget* parent, Qt::WindowFlags f) : QIodeSettings(settings, parent, f)
{
	setupUi(this);

    lineEdit_nb_comments->setText(QString::number(KDBComments().count()));
    lineEdit_nb_equations->setText(QString::number(KDBEquations().count()));
    lineEdit_nb_identities->setText(QString::number(KDBIdentities().count()));
    lineEdit_nb_lists->setText(QString::number(KDBLists().count()));
    lineEdit_nb_scalars->setText(QString::number(KDBScalars().count()));
    lineEdit_nb_tables->setText(QString::number(KDBTables().count()));
    lineEdit_nb_variables->setText(QString::number(KDBVariables().count()));

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

QIodeMenuWorkspaceClear::~QIodeMenuWorkspaceClear()
{
}

void QIodeMenuWorkspaceClear::clear_component(KDBAbstract& kdb, const bool accept)
{
    try
    {
        int i_type = kdb.getIODEType();
        std::string s_type = vIodeTypes[i_type];
        kdb.clear();

        if (accept) this->accept();
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}

void QIodeMenuWorkspaceClear::clear_comments()
{
    KDBComments kdb = KDBComments();
    clear_component(kdb);
}

void QIodeMenuWorkspaceClear::clear_equations()
{
    KDBEquations kdb = KDBEquations();
    clear_component(kdb);
}

void QIodeMenuWorkspaceClear::clear_identities()
{
    KDBIdentities kdb = KDBIdentities();
    clear_component(kdb);
}

void QIodeMenuWorkspaceClear::clear_lists()
{
    KDBLists kdb = KDBLists();
    clear_component(kdb);
}

void QIodeMenuWorkspaceClear::clear_scalars()
{
    KDBScalars kdb = KDBScalars();
    clear_component(kdb);
}

void QIodeMenuWorkspaceClear::clear_tables()
{
    KDBTables kdb = KDBTables();
    clear_component(kdb);
}

void QIodeMenuWorkspaceClear::clear_variables()
{
    KDBVariables kdb = KDBVariables();
    clear_component(kdb);
}

void QIodeMenuWorkspaceClear::clear()
{
    KDBComments kdb_comments = KDBComments();
    KDBEquations kdb_equations = KDBEquations();
    KDBIdentities kdb_identities = KDBIdentities();
    KDBLists kdb_lists = KDBLists();
    KDBScalars kdb_scalars = KDBScalars();
    KDBTables kdb_tables = KDBTables();
    KDBVariables kdb_variables = KDBVariables();

    clear_component(kdb_comments, false);
    clear_component(kdb_equations, false);
    clear_component(kdb_identities, false);
    clear_component(kdb_lists, false);
    clear_component(kdb_scalars, false);
    clear_component(kdb_tables, false);
    clear_component(kdb_variables, false);

    this->accept();
}