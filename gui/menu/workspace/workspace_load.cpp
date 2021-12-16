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

void QIodeMenuWorkspaceLoad::load_component(const EnumIodeType e_type, const bool accept)
{
    try
    {
        std::string s_type = vIodeTypes[e_type];
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
        CPP_WsLoad(filepath.toStdString(), e_type, s_type);

        if (accept) this->accept();
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}

void QIodeMenuWorkspaceLoad::load_comments()
{
    load_component(COMMENTS);
}

void QIodeMenuWorkspaceLoad::load_equations()
{
    load_component(EQUATIONS);
}

void QIodeMenuWorkspaceLoad::load_identities()
{
    load_component(IDENTITIES);
}

void QIodeMenuWorkspaceLoad::load_lists()
{
    load_component(LISTS);
}

void QIodeMenuWorkspaceLoad::load_scalars()
{
    load_component(SCALARS);
}

void QIodeMenuWorkspaceLoad::load_tables()
{
    load_component(TABLES);
}

void QIodeMenuWorkspaceLoad::load_variables()
{
    load_component(VARIABLES);
}

void QIodeMenuWorkspaceLoad::load()
{
    load_component(COMMENTS, false);
    load_component(EQUATIONS, false);
    load_component(IDENTITIES, false);
    load_component(LISTS, false);
    load_component(SCALARS, false);
    load_component(TABLES, false);
    load_component(VARIABLES, false);

    this->accept();
}
