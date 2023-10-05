#include "workspace_copy_into.h"


MenuWorkspaceCopyInto::MenuWorkspaceCopyInto(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

    QList<QString> qIodeTypes;
    for(const std::string& iode_type: vIodeTypes) qIodeTypes << QString::fromStdString(iode_type);

    wComboBoxObjectType = new WrapperComboBox(label_object_type->text(), *comboBox_object_type, REQUIRED_FIELD, qIodeTypes);
    wFileChooserInputFile = new WrapperFileChooser(label_input_file->text(), *fileChooser_input_file, REQUIRED_FIELD, I_COMMENTS_FILE, EXISTING_FILE);
    wSampleEditSampleFrom = new WrapperSampleEdit(label_sample_from->text(), *sampleEdit_sample_from, OPTIONAL_FIELD);
    wSampleEditSampleTo = new WrapperSampleEdit(sampleEdit_sample_to->text(), *sampleEdit_sample_to, OPTIONAL_FIELD);
    wTextEditObjectNames = new WrapperQTextEdit(label_object_names->text(), *textEdit_object_names, OPTIONAL_FIELD);

    mapFields["ObjectType"] = wComboBoxObjectType;
    mapFields["InputFile"] = wFileChooserInputFile;
    mapFields["SampleFrom"] = wSampleEditSampleFrom;
    mapFields["SampleTo"] = wSampleEditSampleTo;
    mapFields["ObjectNames"] = wTextEditObjectNames;

    connect(&wComboBoxObjectType->getQField(), &QComboBox::currentIndexChanged, this, &MenuWorkspaceCopyInto::updateFileChooser);

    className = "MENU_WORKSPACE_COPY_INTO";
    loadSettings();
}

MenuWorkspaceCopyInto::~MenuWorkspaceCopyInto()
{
    delete wComboBoxObjectType;
    delete wFileChooserInputFile;
    delete wSampleEditSampleFrom;
    delete wSampleEditSampleTo;
    delete wTextEditObjectNames;
}

void MenuWorkspaceCopyInto::copy_into_workspace()
{
    try
    {
        int iodeType = wComboBoxObjectType->extractAndVerify();
        std::string input_file = wFileChooserInputFile->extractAndVerify().toStdString();
        std::string from = wSampleEditSampleFrom->extractAndVerify().toStdString();
        std::string to = wSampleEditSampleTo->extractAndVerify().toStdString();
        std::string object_names = wTextEditObjectNames->extractAndVerify().toStdString();

        switch (iodeType)
        {
        case I_COMMENTS:
            Comments.copy_into(input_file, object_names);
            break;
        case I_EQUATIONS:
            Equations.copy_into(input_file, object_names);
            break;
        case I_IDENTITIES:
            Identities.copy_into(input_file, object_names);
            break;
        case I_LISTS:
            Lists.copy_into(input_file, object_names);
            break;
        case I_SCALARS:
            Scalars.copy_into(input_file, object_names);
            break;
        case I_TABLES:
            Tables.copy_into(input_file, object_names);
            break;
        case I_VARIABLES:
            Variables.copy_into(input_file, from, to, object_names);
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

void MenuWorkspaceCopyInto::help()
{
	QDesktopServices::openUrl(url_manual);
}
