#include "workspace_merge_into.h"


MenuWorkspaceMergeInto::MenuWorkspaceMergeInto(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

    QList<QString> qIodeType;
    for(const std::string& iode_type: v_iode_types) qIodeType << QString::fromStdString(iode_type);

    wComboBoxObjectType = new WrapperComboBox(label_object_type->text(), *comboBox_object_type, REQUIRED_FIELD, qIodeType);
    wFileChooserInputFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, REQUIRED_FIELD, FILE_COMMENTS, EXISTING_FILE);

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
        case EQUATIONS:
            Equations.merge_from(input_file);
            break;
        case IDENTITIES:
            Identities.merge_from(input_file);
            break;
        case LISTS:
            Lists.merge_from(input_file);
            break;
        case SCALARS:
            Scalars.merge_from(input_file);
            break;
        case TABLES:
            Tables.merge_from(input_file);
            break;
        case VARIABLES:
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
