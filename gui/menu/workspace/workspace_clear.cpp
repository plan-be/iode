#include "workspace_clear.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from QIodeSettings MUST:
 * 2. call setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from QIodeSettings) with the wrapped field items,
 * 5. initialize the inherited className member (using typeid(this).name())
 * 6. call the loadSettings() method (inherited from QIodeSettings).
 */


QIodeMenuWorkspaceClear::QIodeMenuWorkspaceClear(const QString& project_settings_filepath, QWidget* parent, Qt::WindowFlags f) : QIodeSettings(project_settings_filepath, parent, f)
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

void QIodeMenuWorkspaceClear::clear_objs(const EnumIodeType iode_type, const bool clear_all)
{
    try
    {
        clear_global_kdb(iode_type);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
    if (!clear_all) this->accept();
}

void QIodeMenuWorkspaceClear::clear_comments()
{
    clear_objs(I_COMMENTS, false);
}

void QIodeMenuWorkspaceClear::clear_equations()
{
    clear_objs(I_EQUATIONS, false);
}

void QIodeMenuWorkspaceClear::clear_identities()
{
    clear_objs(I_IDENTITIES, false);
}

void QIodeMenuWorkspaceClear::clear_lists()
{
    clear_objs(I_LISTS, false);
}

void QIodeMenuWorkspaceClear::clear_scalars()
{
    clear_objs(I_SCALARS, false);
}

void QIodeMenuWorkspaceClear::clear_tables()
{
    clear_objs(I_TABLES, false);
}

void QIodeMenuWorkspaceClear::clear_variables()
{
    clear_objs(I_VARIABLES, false);
}

void QIodeMenuWorkspaceClear::clear()
{
    clear_objs(I_COMMENTS, true);
    clear_objs(I_EQUATIONS, true);
    clear_objs(I_IDENTITIES, true);
    clear_objs(I_LISTS, true);
    clear_objs(I_SCALARS, true);
    clear_objs(I_TABLES, true);
    clear_objs(I_VARIABLES, true);

    this->accept();
}