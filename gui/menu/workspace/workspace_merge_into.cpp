#include "workspace_merge_into.h"


QIodeMenuWorkspaceMergeInto::QIodeMenuWorkspaceMergeInto(const QString& settings_filepath, QWidget* parent, Qt::WindowFlags f) : 
    QIodeSettings(settings_filepath, parent, f)
{
    setupUi(this);

    QList<QString> qIodeTypes;
    for(const std::string& iode_type: vIodeTypes) qIodeTypes << QString::fromStdString(iode_type);

    wComboBoxObjectType = new WrapperComboBox(label_object_type->text(), *comboBox_object_type, REQUIRED_FIELD, qIodeTypes);
    wFileChooserInputFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, REQUIRED_FIELD, I_COMMENTS_FILE, EXISTING_FILE);

    mapFields["ObjectType"] = wComboBoxObjectType;
    mapFields["InputFile"] = wFileChooserInputFile;

    connect(&wComboBoxObjectType->getQField(), &QComboBox::currentIndexChanged, this, &QIodeMenuWorkspaceMergeInto::updateFileChooser);

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

QIodeMenuWorkspaceMergeInto::~QIodeMenuWorkspaceMergeInto()
{
    delete wComboBoxObjectType;
    delete wFileChooserInputFile;
}

void QIodeMenuWorkspaceMergeInto::merge_into_workspace()
{
    try
    {
        int iodeType = wComboBoxObjectType->extractAndVerify();
        std::string input_file = wFileChooserInputFile->extractAndVerify().toStdString();

        KDBComments kdb_comments;
        KDBEquations kdb_equations;
        KDBIdentities kdb_identities;
        KDBLists kdb_lists;
        KDBScalars kdb_scalars;
        KDBTables kdb_tables;
        KDBVariables kdb_variables;
        switch (iodeType)
        {
        case I_COMMENTS:
            kdb_comments.merge_into(input_file);
            break;
        case I_EQUATIONS:
            kdb_equations.merge_into(input_file);
            break;
        case I_IDENTITIES:
            kdb_identities.merge_into(input_file);
            break;
        case I_LISTS:
            kdb_lists.merge_into(input_file);
            break;
        case I_SCALARS:
            kdb_scalars.merge_into(input_file);
            break;
        case I_TABLES:
            kdb_tables.merge_into(input_file);
            break;
        case I_VARIABLES:
            kdb_variables.merge_into(input_file);
            break;
        default:
            break;
        }
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }

    this->accept();
}

void QIodeMenuWorkspaceMergeInto::help()
{
	QDesktopServices::openUrl(url_manual);
}
