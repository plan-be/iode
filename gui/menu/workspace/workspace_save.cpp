#include "workspace_save.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from QIodeSettings MUST:
 * 2. call setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from QIodeSettings) with the wrapped field items,
 * 5. initialize the inherited className member (using typeid(this).name())
 * 6. call the loadSettings() method (inherited from QIodeSettings).
 */


QIodeMenuWorkspaceSave::QIodeMenuWorkspaceSave(QSettings& settings, QWidget* parent, Qt::WindowFlags f) : QIodeSettings(settings, parent, f)
{
	setupUi(this);

    mapFields["Comments"] = new WrapperFileChooser(pushButton_comments->text(), *fileChooser_comments, OPTIONAL_FIELD, COMMENTS_FILE, FILE_MAY_EXIST);
    mapFields["Equations"] = new WrapperFileChooser(pushButton_equations->text(), *fileChooser_equations, OPTIONAL_FIELD, EQUATIONS_FILE, FILE_MAY_EXIST);
    mapFields["Identities"] = new WrapperFileChooser(pushButton_identities->text(), *fileChooser_identities, OPTIONAL_FIELD, IDENTITIES_FILE, FILE_MAY_EXIST);
    mapFields["Lists"] = new WrapperFileChooser(pushButton_lists->text(), *fileChooser_lists, OPTIONAL_FIELD, LISTS_FILE, FILE_MAY_EXIST);
    mapFields["Scalars"] = new WrapperFileChooser(pushButton_scalars->text(), *fileChooser_scalars, OPTIONAL_FIELD, SCALARS_FILE, FILE_MAY_EXIST);
    mapFields["Tables"] = new WrapperFileChooser(pushButton_tables->text(), *fileChooser_tables, OPTIONAL_FIELD, TABLES_FILE, FILE_MAY_EXIST);
    mapFields["Variables"] = new WrapperFileChooser(pushButton_variables->text(), *fileChooser_variables, OPTIONAL_FIELD, VARIABLES_FILE, FILE_MAY_EXIST);

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

QIodeMenuWorkspaceSave::~QIodeMenuWorkspaceSave()
{
}

void QIodeMenuWorkspaceSave::save_component(const QString& type, const bool accept)
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
            QMessageBox::warning(this, tr("WARNING"), QString("Cannot save %1. Filepath is empty.").arg(type));
            return;
        }
        CPP_WsSave(filepath.toStdString(), i_type, type.toStdString());

        if (accept) this->accept();
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}

void QIodeMenuWorkspaceSave::save_comments()
{
    save_component("Comments");
}

void QIodeMenuWorkspaceSave::save_equations()
{
    save_component("Equations");
}

void QIodeMenuWorkspaceSave::save_identities()
{
    save_component("Identities");
}

void QIodeMenuWorkspaceSave::save_lists()
{
    save_component("Lists");
}

void QIodeMenuWorkspaceSave::save_scalars()
{
    save_component("Scalars");
}

void QIodeMenuWorkspaceSave::save_tables()
{
    save_component("Tables");
}

void QIodeMenuWorkspaceSave::save_variables()
{
    save_component("Variables");
}

void QIodeMenuWorkspaceSave::save()
{
    save_component("Comments", false);
    save_component("Equations", false);
    save_component("Identities", false);
    save_component("Lists", false);
    save_component("Scalars", false);
    save_component("Tables", false);
    save_component("Variables", false);

    this->accept();
}
