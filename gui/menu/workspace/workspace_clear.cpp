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

void QIodeMenuWorkspaceClear::clear_component(const QString& type, const bool accept)
{
    try
    {
        EnumIodeType i_type = static_cast<EnumIodeType>(qmapIodeTypes.value(type));
        CPP_WsClear(i_type, type.toStdString());

        if (accept) this->accept();
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}

void QIodeMenuWorkspaceClear::clear_comments()
{
    clear_component("Comments");
}

void QIodeMenuWorkspaceClear::clear_equations()
{
    clear_component("Equations");
}

void QIodeMenuWorkspaceClear::clear_identities()
{
    clear_component("Identities");
}

void QIodeMenuWorkspaceClear::clear_lists()
{
    clear_component("Lists");
}

void QIodeMenuWorkspaceClear::clear_scalars()
{
    clear_component("Scalars");
}

void QIodeMenuWorkspaceClear::clear_tables()
{
    clear_component("Tables");
}

void QIodeMenuWorkspaceClear::clear_variables()
{
    clear_component("Variables");
}

void QIodeMenuWorkspaceClear::clear()
{
    clear_component("Comments", false);
    clear_component("Equations", false);
    clear_component("Identities", false);
    clear_component("Lists", false);
    clear_component("Scalars", false);
    clear_component("Tables", false);
    clear_component("Variables", false);

    this->accept();
}
