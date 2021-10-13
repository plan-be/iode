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

    mapFields["Comments"] = new WrapperFileChooser(pushButton_comments->text(), *fileChooser_comments, OPTIONAL_FIELD, COMMENTS_FILE, EXISTING_FILE);
    mapFields["Equations"] = new WrapperFileChooser(pushButton_equations->text(), *fileChooser_equations, OPTIONAL_FIELD, EQUATIONS_FILE, EXISTING_FILE);
    mapFields["Identities"] = new WrapperFileChooser(pushButton_identities->text(), *fileChooser_identities, OPTIONAL_FIELD, IDENTITIES_FILE, EXISTING_FILE);
    mapFields["Lists"] = new WrapperFileChooser(pushButton_lists->text(), *fileChooser_lists, OPTIONAL_FIELD, LISTS_FILE, EXISTING_FILE);
    mapFields["Scalars"] = new WrapperFileChooser(pushButton_scalars->text(), *fileChooser_scalars, OPTIONAL_FIELD, SCALARS_FILE, EXISTING_FILE);
    mapFields["Tables"] = new WrapperFileChooser(pushButton_tables->text(), *fileChooser_tables, OPTIONAL_FIELD, TABLES_FILE, EXISTING_FILE);
    mapFields["Variables"] = new WrapperFileChooser(pushButton_variables->text(), *fileChooser_variables, OPTIONAL_FIELD, VARIABLES_FILE, EXISTING_FILE);

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

QIodeMenuWorkspaceLoad::~QIodeMenuWorkspaceLoad()
{
}

void QIodeMenuWorkspaceLoad::load_component(const QString& type, const bool accept)
{
    try
    {
        EnumIodeType i_type = static_cast<EnumIodeType>(qmapIodeTypes.value(type));
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields.value(type));
        QString filepath = field_filepath->extractAndVerify();
        // accept = true means that the users clicked on an individual "Load XXX" button.
        // In that case and if the filepath is empty, we show warning box
        if (accept && filepath.isEmpty())
        {
            QMessageBox::warning(this, tr("WARNING"), QString("Cannot load %1. Filepath is empty.").arg(type));
            return;
        }
        CPP_WsLoad(filepath.toStdString(), i_type, type.toStdString());

        if (accept) this->accept();
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}

void QIodeMenuWorkspaceLoad::load_comments()
{
    load_component("Comments");
}

void QIodeMenuWorkspaceLoad::load_equations()
{
    load_component("Equations");
}

void QIodeMenuWorkspaceLoad::load_identities()
{
    load_component("Identities");
}

void QIodeMenuWorkspaceLoad::load_lists()
{
    load_component("Lists");
}

void QIodeMenuWorkspaceLoad::load_scalars()
{
    load_component("Scalars");
}

void QIodeMenuWorkspaceLoad::load_tables()
{
    load_component("Tables");
}

void QIodeMenuWorkspaceLoad::load_variables()
{
    load_component("Variables");
}

void QIodeMenuWorkspaceLoad::load()
{
    load_component("Comments", false);
    load_component("Equations", false);
    load_component("Identities", false);
    load_component("Lists", false);
    load_component("Scalars", false);
    load_component("Tables", false);
    load_component("Variables", false);

    this->accept();
}
