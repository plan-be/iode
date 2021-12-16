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

    lineEdit_nb_comments->setText(QString::number(Comments().count()));
    lineEdit_nb_equations->setText(QString::number(Equations().count()));
    lineEdit_nb_identities->setText(QString::number(Identities().count()));
    lineEdit_nb_lists->setText(QString::number(Lists().count()));
    lineEdit_nb_scalars->setText(QString::number(Scalars().count()));
    lineEdit_nb_tables->setText(QString::number(Tables().count()));
    lineEdit_nb_variables->setText(QString::number(Variables().count()));

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

QIodeMenuWorkspaceClear::~QIodeMenuWorkspaceClear()
{
}

void QIodeMenuWorkspaceClear::clear_component(const EnumIodeType e_type, const bool accept)
{
    try
    {
        std::string s_type = vIodeTypes[e_type];
        CPP_WsClear(e_type, s_type);

        if (accept) this->accept();
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}

void QIodeMenuWorkspaceClear::clear_comments()
{
    clear_component(COMMENTS);
}

void QIodeMenuWorkspaceClear::clear_equations()
{
    clear_component(EQUATIONS);
}

void QIodeMenuWorkspaceClear::clear_identities()
{
    clear_component(IDENTITIES);
}

void QIodeMenuWorkspaceClear::clear_lists()
{
    clear_component(LISTS);
}

void QIodeMenuWorkspaceClear::clear_scalars()
{
    clear_component(SCALARS);
}

void QIodeMenuWorkspaceClear::clear_tables()
{
    clear_component(TABLES);
}

void QIodeMenuWorkspaceClear::clear_variables()
{
    clear_component(VARIABLES);
}

void QIodeMenuWorkspaceClear::clear()
{
    clear_component(COMMENTS, false);
    clear_component(EQUATIONS, false);
    clear_component(IDENTITIES, false);
    clear_component(LISTS, false);
    clear_component(SCALARS, false);
    clear_component(TABLES, false);
    clear_component(VARIABLES, false);

    this->accept();
}
