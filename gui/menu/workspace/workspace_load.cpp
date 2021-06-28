
#include "workspace_load.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from MixingSettings MUST:
 * 1. instantiate the pointer *ui,
 * 2. call ui->setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from MixinSettings) with the wrapped field items,
 * 5. initialize the inherited className member (using typeid(this).name())
 * 6. call the loadSettings() method (inherited from MixinSettings).
 */
LoadWorkspaceDialog::LoadWorkspaceDialog(QSettings& settings, QWidget* parent, Qt::WindowFlags f) : MixinSettings(settings, parent, f)
{
    ui = new Ui::load_workspace();
    ui->setupUi(this);

    mapFields["Comments"] = new WrapperFileChooser(ui->pushButton_comments->text(), *ui->fileChooser_comments, Optional, CommentsFile, ExistingFile);
    mapFields["Equations"] = new WrapperFileChooser(ui->pushButton_equations->text(), *ui->fileChooser_equations, Optional, EquationsFile, ExistingFile);
    mapFields["Identities"] = new WrapperFileChooser(ui->pushButton_identities->text(), *ui->fileChooser_identities, Optional, IdentitiesFile, ExistingFile);
    mapFields["Lists"] = new WrapperFileChooser(ui->pushButton_lists->text(), *ui->fileChooser_lists, Optional, ListsFile, ExistingFile);
    mapFields["Scalars"] = new WrapperFileChooser(ui->pushButton_scalars->text(), *ui->fileChooser_scalars, Optional, ScalarsFile, ExistingFile);
    mapFields["Tables"] = new WrapperFileChooser(ui->pushButton_tables->text(), *ui->fileChooser_tables, Optional, TablesFile, ExistingFile);
    mapFields["Variables"] = new WrapperFileChooser(ui->pushButton_variables->text(), *ui->fileChooser_variables, Optional, VariablesFile, ExistingFile);

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

LoadWorkspaceDialog::~LoadWorkspaceDialog()
{
    delete ui;
}

void LoadWorkspaceDialog::load_component(const QString& type, const bool accept)
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

void LoadWorkspaceDialog::load_comments()
{
    load_component("Comments");
}

void LoadWorkspaceDialog::load_equations()
{
    load_component("Equations");
}

void LoadWorkspaceDialog::load_identities()
{
    load_component("Identities");
}

void LoadWorkspaceDialog::load_lists()
{
    load_component("Lists");
}

void LoadWorkspaceDialog::load_scalars()
{
    load_component("Scalars");
}

void LoadWorkspaceDialog::load_tables()
{
    load_component("Tables");
}

void LoadWorkspaceDialog::load_variables()
{
    load_component("Variables");
}

void LoadWorkspaceDialog::load()
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
