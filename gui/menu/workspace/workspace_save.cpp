#include "workspace_save.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from QIodeSettings MUST:
 * 2. call setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from QIodeSettings) with the wrapped field items,
 * 5. initialize the inherited className member (using typeid(this).name())
 * 6. call the loadSettings() method (inherited from QIodeSettings).
 */


QIodeMenuWorkspaceSave::QIodeMenuWorkspaceSave(const QString& settings_filepath, QWidget* parent, Qt::WindowFlags f) : QIodeSettings(settings_filepath, parent, f)
{
	setupUi(this);

    mapFields["Comments"] = new WrapperFileChooser(pushButton_comments->text(), *fileChooser_comments, OPTIONAL_FIELD, I_COMMENTS_FILE, FILE_MAY_EXIST);
    mapFields["Equations"] = new WrapperFileChooser(pushButton_equations->text(), *fileChooser_equations, OPTIONAL_FIELD, I_EQUATIONS_FILE, FILE_MAY_EXIST);
    mapFields["Identities"] = new WrapperFileChooser(pushButton_identities->text(), *fileChooser_identities, OPTIONAL_FIELD, I_IDENTITIES_FILE, FILE_MAY_EXIST);
    mapFields["Lists"] = new WrapperFileChooser(pushButton_lists->text(), *fileChooser_lists, OPTIONAL_FIELD, I_LISTS_FILE, FILE_MAY_EXIST);
    mapFields["Scalars"] = new WrapperFileChooser(pushButton_scalars->text(), *fileChooser_scalars, OPTIONAL_FIELD, I_SCALARS_FILE, FILE_MAY_EXIST);
    mapFields["Tables"] = new WrapperFileChooser(pushButton_tables->text(), *fileChooser_tables, OPTIONAL_FIELD, I_TABLES_FILE, FILE_MAY_EXIST);
    mapFields["Variables"] = new WrapperFileChooser(pushButton_variables->text(), *fileChooser_variables, OPTIONAL_FIELD, I_VARIABLES_FILE, FILE_MAY_EXIST);

    nb_comments = KDBComments().count();
    nb_equations = KDBEquations().count();
    nb_identities = KDBIdentities().count();
    nb_lists = KDBLists().count();
    nb_scalars = KDBScalars().count();
    nb_tables = KDBTables().count();
    nb_variables = KDBVariables().count();

    lineEdit_nb_comments->setText(QString::number(nb_comments));
    lineEdit_nb_equations->setText(QString::number(nb_equations));
    lineEdit_nb_identities->setText(QString::number(nb_lists));
    lineEdit_nb_lists->setText(QString::number(nb_lists));
    lineEdit_nb_scalars->setText(QString::number(nb_scalars));
    lineEdit_nb_tables->setText(QString::number(nb_tables));
    lineEdit_nb_variables->setText(QString::number(nb_variables));

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

QIodeMenuWorkspaceSave::~QIodeMenuWorkspaceSave()
{
}

void QIodeMenuWorkspaceSave::save_objs(const EnumIodeType iode_type, const bool save_all)
{
    try
    {
        QString str_iode_type = QString::fromStdString(vIodeTypes[iode_type]);
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields[str_iode_type]);
        QString filepath = field_filepath->extractAndVerify();
        // save_all means that the users clicked on an the "Save" button to load all files at once.
        // In that case and if the filepath is empty, the save_xxx() method shows a warning box
        if (save_all && filepath.isEmpty())
        {
            if (nb_comments > 0) QMessageBox::warning(this, tr("Warning"), "No filepath provided for Comments.\nComments will not be saved.");
        }
        else
        {
            save_global_kdb(iode_type, filepath.toStdString());
        }
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!save_all) this->accept();
}

void QIodeMenuWorkspaceSave::save_comments()
{
    save_objs(I_COMMENTS, false);
}

void QIodeMenuWorkspaceSave::save_equations()
{
    save_objs(I_EQUATIONS, false);
}

void QIodeMenuWorkspaceSave::save_identities()
{
    save_objs(I_IDENTITIES, false);
}

void QIodeMenuWorkspaceSave::save_lists()
{
    save_objs(I_LISTS, false);
}

void QIodeMenuWorkspaceSave::save_scalars()
{
    save_objs(I_SCALARS, false);
}

void QIodeMenuWorkspaceSave::save_tables()
{
    save_objs(I_TABLES, false);
}

void QIodeMenuWorkspaceSave::save_variables()
{
    save_objs(I_VARIABLES, false);
}

void QIodeMenuWorkspaceSave::save()
{
    save_objs(I_COMMENTS, true);
    save_objs(I_EQUATIONS, true);
    save_objs(I_IDENTITIES, true);
    save_objs(I_LISTS, true);
    save_objs(I_SCALARS, true);
    save_objs(I_TABLES, true);
    save_objs(I_VARIABLES, true);

    this->accept();
}