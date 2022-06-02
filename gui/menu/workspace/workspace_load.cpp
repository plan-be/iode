#include "workspace_load.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from QIodeSettings MUST:
 * 2. call setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from QIodeSettings) with the wrapped field items,
 * 5. initialize the inherited className member (using typeid(this).name())
 * 6. call the loadSettings() method (inherited from QIodeSettings).
 */


QIodeMenuWorkspaceLoad::QIodeMenuWorkspaceLoad(const QString& settings_filepath, QWidget* parent, Qt::WindowFlags f) : QIodeSettings(settings_filepath, parent, f)
{
	setupUi(this);

    load_all = false;

    mapFields["Comments"] = new WrapperFileChooser(pushButton_comments->text(), *fileChooser_comments, OPTIONAL_FIELD, I_COMMENTS_FILE, EXISTING_FILE);
    mapFields["Equations"] = new WrapperFileChooser(pushButton_equations->text(), *fileChooser_equations, OPTIONAL_FIELD, I_EQUATIONS_FILE, EXISTING_FILE);
    mapFields["Identities"] = new WrapperFileChooser(pushButton_identities->text(), *fileChooser_identities, OPTIONAL_FIELD, I_IDENTITIES_FILE, EXISTING_FILE);
    mapFields["Lists"] = new WrapperFileChooser(pushButton_lists->text(), *fileChooser_lists, OPTIONAL_FIELD, I_LISTS_FILE, EXISTING_FILE);
    mapFields["Scalars"] = new WrapperFileChooser(pushButton_scalars->text(), *fileChooser_scalars, OPTIONAL_FIELD, I_SCALARS_FILE, EXISTING_FILE);
    mapFields["Tables"] = new WrapperFileChooser(pushButton_tables->text(), *fileChooser_tables, OPTIONAL_FIELD, I_TABLES_FILE, EXISTING_FILE);
    mapFields["Variables"] = new WrapperFileChooser(pushButton_variables->text(), *fileChooser_variables, OPTIONAL_FIELD, I_VARIABLES_FILE, EXISTING_FILE);

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

QIodeMenuWorkspaceLoad::~QIodeMenuWorkspaceLoad()
{
}

void QIodeMenuWorkspaceLoad::load_comments()
{
    KDBComments kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Comments"]);
        QString filepath = field_filepath->extractAndVerify();
        // load_all means that the users clicked on an the "Load" button to load all files at once.
        // In that case and if the filepath is empty, the load_xxx() method does nothing
        if (load_all && filepath.isEmpty()) return;
        kdb.load(filepath.toStdString());
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!load_all) this->accept();
}

void QIodeMenuWorkspaceLoad::load_equations()
{
    KDBEquations kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Equations"]);
        QString filepath = field_filepath->extractAndVerify();
        if (load_all && filepath.isEmpty()) return;
        kdb.load(filepath.toStdString());
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!load_all) this->accept();
}

void QIodeMenuWorkspaceLoad::load_identities()
{
    KDBIdentities kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Identities"]);
        QString filepath = field_filepath->extractAndVerify();
        if (load_all && filepath.isEmpty()) return;
        kdb.load(filepath.toStdString());
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!load_all) this->accept();
}

void QIodeMenuWorkspaceLoad::load_lists()
{
    KDBLists kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Lists"]);
        QString filepath = field_filepath->extractAndVerify();
        if (load_all && filepath.isEmpty()) return;
        kdb.load(filepath.toStdString());
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!load_all) this->accept();
}

void QIodeMenuWorkspaceLoad::load_scalars()
{
    KDBScalars kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Scalars"]);
        QString filepath = field_filepath->extractAndVerify();
        if (load_all && filepath.isEmpty()) return;
        kdb.load(filepath.toStdString());
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!load_all) this->accept();
}

void QIodeMenuWorkspaceLoad::load_tables()
{
    KDBTables kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Tables"]);
        QString filepath = field_filepath->extractAndVerify();
        if (load_all && filepath.isEmpty()) return;
        kdb.load(filepath.toStdString());
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!load_all) this->accept();
}

void QIodeMenuWorkspaceLoad::load_variables()
{
    KDBVariables kdb;
    try
    {
        WrapperFileChooser* field_filepath = static_cast<WrapperFileChooser*>(mapFields["Variables"]);
        QString filepath = field_filepath->extractAndVerify();
        if (load_all && filepath.isEmpty()) return;
        kdb.load(filepath.toStdString());
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    if (!load_all) this->accept();
}

void QIodeMenuWorkspaceLoad::load()
{
    load_all = true;

    load_comments();
    load_equations();
    load_identities();
    load_lists();
    load_scalars();
    load_tables();
    load_variables();

    this->accept();
}
