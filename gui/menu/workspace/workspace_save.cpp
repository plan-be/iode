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

    save_all = false;

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

void QIodeMenuWorkspaceSave::save_comments()
{
    KDBComments kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Comments"]);
        QString filepath = field_filepath->extractAndVerify();
        // save_all means that the users clicked on an the "Save" button to load all files at once.
        // In that case and if the filepath is empty, the save_xxx() method shows a warning box
        if (save_all && filepath.isEmpty())
        {
            if (nb_comments > 0) QMessageBox::warning(this, tr("Warning"), "No filepath provided for Comments.\nComments will not be saved.");
        }
        else
        {
            kdb.save(filepath.toStdString());
        }
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!save_all) this->accept();
}

void QIodeMenuWorkspaceSave::save_equations()
{
    KDBEquations kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Equations"]);
        QString filepath = field_filepath->extractAndVerify();
        if (save_all && filepath.isEmpty())
        {
            if (nb_equations > 0) QMessageBox::warning(this, tr("Warning"), "No filepath provided for Equations.\nEquations will not be saved.");
        }
        else
        {
            kdb.save(filepath.toStdString());
        }
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!save_all) this->accept();
}

void QIodeMenuWorkspaceSave::save_identities()
{
    KDBIdentities kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Identities"]);
        QString filepath = field_filepath->extractAndVerify();
        if (save_all && filepath.isEmpty())
        {
            if (nb_identities > 0) QMessageBox::warning(this, tr("Warning"), "No filepath provided for Identities.\nIdentities will not be saved.");
        }
        else
        {
            kdb.save(filepath.toStdString());
        }
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!save_all) this->accept();
}

void QIodeMenuWorkspaceSave::save_lists()
{
    KDBLists kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Lists"]);
        QString filepath = field_filepath->extractAndVerify();
        if (save_all && filepath.isEmpty())
        {
            if (nb_lists > 0) QMessageBox::warning(this, tr("Warning"), "No filepath provided for Lists.\nLists will not be saved.");
        }
        else
        {
            kdb.save(filepath.toStdString());
        }
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!save_all) this->accept();
}

void QIodeMenuWorkspaceSave::save_scalars()
{
    KDBScalars kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Scalars"]);
        QString filepath = field_filepath->extractAndVerify();
        if (save_all && filepath.isEmpty())
        {
            if (nb_scalars > 0) QMessageBox::warning(this, tr("Warning"), "No filepath provided for Scalars.\nScalars will not be saved.");
        }
        else
        {
            kdb.save(filepath.toStdString());
        }
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!save_all) this->accept();
}

void QIodeMenuWorkspaceSave::save_tables()
{
    KDBTables kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Tables"]);
        QString filepath = field_filepath->extractAndVerify();
        if (save_all && filepath.isEmpty())
        {
            if (nb_tables > 0) QMessageBox::warning(this, tr("Warning"), "No filepath provided for Tables.\nTables will not be saved.");
        }
        else
        {
            kdb.save(filepath.toStdString());
        }
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!save_all) this->accept();
}

void QIodeMenuWorkspaceSave::save_variables()
{
    KDBVariables kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Variables"]);
        QString filepath = field_filepath->extractAndVerify();
        if (save_all && filepath.isEmpty())
        {
            if (nb_variables > 0) QMessageBox::warning(this, tr("Warning"), "No filepath provided for Variables.\nVariables will not be saved.");
        }
        else
        {
            kdb.save(filepath.toStdString());
        }
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!save_all) this->accept();
}

void QIodeMenuWorkspaceSave::save()
{
    save_all = true;

    save_comments();
    save_equations();
    save_identities();
    save_lists();
    save_scalars();
    save_tables();
    save_variables();

    this->accept();
}