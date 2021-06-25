
#include "workspace_save.h"

/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from MixingSettings MUST:
 * 1. instantiate the pointer *ui,
 * 2. call ui->setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from MixinSettings) with the wrapped field items,
 * 5. initialize the inherited className member (using typeid(this).name())
 * 6. call the loadSettings() method (inherited from MixinSettings).
 */
SaveWorkspaceDialog::SaveWorkspaceDialog(QSettings& settings, QWidget* parent, Qt::WindowFlags f) : MixinSettings(settings, parent, f)
{
    ui = new Ui::save_workspace();
    ui->setupUi(this);

    mapFiles["Comments"] = new WrapperFileChooser(ui->pushButton_comments->text(), *ui->fileChooser_comments, Optional, CommentsFile, FileMayExist);
    mapFiles["Equations"] = new WrapperFileChooser(ui->pushButton_equations->text(), *ui->fileChooser_equations, Optional, EquationsFile, FileMayExist);
    mapFiles["Identities"] = new WrapperFileChooser(ui->pushButton_identities->text(), *ui->fileChooser_identities, Optional, IdentitiesFile, FileMayExist);
    mapFiles["Lists"] = new WrapperFileChooser(ui->pushButton_lists->text(), *ui->fileChooser_lists, Optional, ListsFile, FileMayExist);
    mapFiles["Scalars"] = new WrapperFileChooser(ui->pushButton_scalars->text(), *ui->fileChooser_scalars, Optional, ScalarsFile, FileMayExist);
    mapFiles["Tables"] = new WrapperFileChooser(ui->pushButton_tables->text(), *ui->fileChooser_tables, Optional, TablesFile, FileMayExist);
    mapFiles["Variables"] = new WrapperFileChooser(ui->pushButton_variables->text(), *ui->fileChooser_variables, Optional, VariablesFile, FileMayExist);

    ui->lineEdit_nb_comments->setText(QString::number(get_nb_elements_WS(Comments)));
    ui->lineEdit_nb_equations->setText(QString::number(get_nb_elements_WS(Equations)));
    ui->lineEdit_nb_identities->setText(QString::number(get_nb_elements_WS(Identities)));
    ui->lineEdit_nb_lists->setText(QString::number(get_nb_elements_WS(Lists)));
    ui->lineEdit_nb_scalars->setText(QString::number(get_nb_elements_WS(Scalars)));
    ui->lineEdit_nb_tables->setText(QString::number(get_nb_elements_WS(Tables)));
    ui->lineEdit_nb_variables->setText(QString::number(get_nb_elements_WS(Variables)));
 
    QMap<QString, WrapperFileChooser*>::iterator j;
    for (j = mapFiles.begin(); j != mapFiles.end(); ++j) mapFields[j.key()] = j.value();

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

SaveWorkspaceDialog::~SaveWorkspaceDialog()
{
    delete ui;
}

void SaveWorkspaceDialog::save_component(const QString& type, const bool accept)
{
    try
    {
        int i_type = qmapIodeTypes.value(type);
        WrapperFileChooser* field_filepath = mapFiles.value(type);
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

void SaveWorkspaceDialog::save_comments()
{
    save_component("Comments");
}

void SaveWorkspaceDialog::save_equations()
{
    save_component("Equations");
}

void SaveWorkspaceDialog::save_identities()
{
    save_component("Identities");
}

void SaveWorkspaceDialog::save_lists()
{
    save_component("Lists");
}

void SaveWorkspaceDialog::save_scalars()
{
    save_component("Scalars");
}

void SaveWorkspaceDialog::save_tables()
{
    save_component("Tables");
}

void SaveWorkspaceDialog::save_variables()
{
    save_component("Variables");
}

void SaveWorkspaceDialog::save()
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
