#include "workspace_merge_into.h"


MenuWorkspaceMergeInto::MenuWorkspaceMergeInto(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

    QList<QString> qIodeTypes;
    for(const std::string& iode_type: vIodeTypes) qIodeTypes << QString::fromStdString(iode_type);

    wComboBoxObjectType = new WrapperComboBox(label_object_type->text(), *comboBox_object_type, REQUIRED_FIELD, qIodeTypes);
    wFileChooserInputFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, REQUIRED_FIELD, COMMENTS_FILE, EXISTING_FILE);

    mapFields["ObjectType"] = wComboBoxObjectType;
    mapFields["InputFile"] = wFileChooserInputFile;

    connect(&wComboBoxObjectType->getQField(), &QComboBox::currentIndexChanged, this, &MenuWorkspaceMergeInto::updateFileChooser);

    className = "MENU_WORKSPACE_MERGE_INTO";
    loadSettings();
}

MenuWorkspaceMergeInto::~MenuWorkspaceMergeInto()
{
    delete wComboBoxObjectType;
    delete wFileChooserInputFile;
}

void MenuWorkspaceMergeInto::merge_into_workspace()
{
    try
    {
        int iodeType = wComboBoxObjectType->extractAndVerify();
        std::string input_file = wFileChooserInputFile->extractAndVerify().toStdString();

        switch (iodeType)
        {
        case COMMENTS:
            Comments.merge_from(input_file);
            break;
        case I_EQUATIONS:
            Equations.merge_from(input_file);
            break;
        case I_IDENTITIES:
            Identities.merge_from(input_file);
            break;
        case I_LISTS:
            Lists.merge_from(input_file);
            break;
        case I_SCALARS:
            Scalars.merge_from(input_file);
            break;
        case I_TABLES:
            Tables.merge_from(input_file);
            break;
        case I_VARIABLES:
            Variables.merge_from(input_file);
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

void MenuWorkspaceMergeInto::help()
{
	QDesktopServices::openUrl(url_manual);
}
