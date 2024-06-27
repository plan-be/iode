#include "duplicate.h"


MenuDataDuplicateObj::MenuDataDuplicateObj(QWidget* parent)
    : IodeSettingsDialog(parent)
{
    setupUi(this);

    completer = new IodeCompleter(false, false, COMMENTS, this);
    lineEdit_obj_name->setCompleter(completer);
    comboBox_iode_types->setCurrentIndex(0);

    QStringList listIodeTypes;
    for(const std::string& iode_type : vIodeTypes) listIodeTypes << QString::fromStdString(iode_type);

    wComboIodeTypes = new WrapperComboBox(label_iode_types->text(), *comboBox_iode_types, REQUIRED_FIELD, listIodeTypes);
    wObjName = new WrapperQLineEdit(label_obj_name->text(), *lineEdit_obj_name, REQUIRED_FIELD);
    wDupObjName = new WrapperQLineEdit(label_dup_obj_name->text(), *lineEdit_dup_obj_name, REQUIRED_FIELD);

    mapFields["IodeTypes"]  = wComboIodeTypes;
    mapFields["ObjName"]    = wObjName;
    mapFields["DupObjName"] = wDupObjName;

    connect(comboBox_iode_types, &QComboBox::currentIndexChanged, lineEdit_obj_name, &IodeAutoCompleteLineEdit::setIodeType);

    className = "MENU_DATA_DUPLICATE_OBJ";
    loadSettings();
}

MenuDataDuplicateObj::~MenuDataDuplicateObj()
{
    delete wComboIodeTypes;
    delete wObjName;
    delete wDupObjName;

    delete completer;
}

void MenuDataDuplicateObj::duplicate()
{
    try
    {
        int iode_type = wComboIodeTypes->extractAndVerify();
        std::string obj_name = wObjName->extractAndVerify().toStdString();
        std::string dup_obj_name = wDupObjName->extractAndVerify().toStdString();

        switch (iode_type)
        {
        case COMMENTS:
            Comments.add(dup_obj_name, Comments.copy(obj_name));
            break;
        case EQUATIONS:
            throw IodeException("Cannot duplicate an equation");
            break;     
        case IDENTITIES:
            Identities.add(dup_obj_name, Identities.copy(obj_name));
            break; 
        case I_LISTS:
            Lists.add(dup_obj_name, Lists.copy(obj_name));
            break; 
        case I_SCALARS:
            Scalars.add(dup_obj_name, Scalars.copy(obj_name));
            break; 
        case I_TABLES:
            Tables.add(dup_obj_name, Tables.copy(obj_name));
            break; 
        case I_VARIABLES:
            Variables.add(dup_obj_name, Variables.copy(obj_name));
            break; 
        default:
            break;
        }

        this->accept();
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
    }
}

void MenuDataDuplicateObj::help()
{
	QDesktopServices::openUrl(url_manual);
}
