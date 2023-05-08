#include "duplicate.h"


QIodeMenuDataDuplicateObj::QIodeMenuDataDuplicateObj(const QString& project_settings_filepath, QWidget* parent, Qt::WindowFlags f)
    : QIodeSettings(project_settings_filepath, parent, f)
{
    setupUi(this);

    completer = new QIodeCompleter(false, false, I_COMMENTS, this);
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

    connect(comboBox_iode_types, &QComboBox::currentIndexChanged, lineEdit_obj_name, &QIodeCompleteLineEdit::setIodeType);

    className = "MENU_DATA_DUPLICATE_OBJ";
    loadSettings();
}

QIodeMenuDataDuplicateObj::~QIodeMenuDataDuplicateObj()
{
    delete wComboIodeTypes;
    delete wObjName;
    delete wDupObjName;

    delete completer;
}

void QIodeMenuDataDuplicateObj::duplicate()
{
    try
    {
        int iode_type = wComboIodeTypes->extractAndVerify();
        std::string obj_name = wObjName->extractAndVerify().toStdString();
        std::string dup_obj_name = wDupObjName->extractAndVerify().toStdString();

        KDBComments kdb_cmt;
        KDBEquations kdb_eqs;
        KDBIdentities kdb_idt;
        KDBLists kdb_lst;
        KDBScalars kdb_scl;
        KDBTables kdb_tbl;
        KDBVariables kdb_var;

        switch (iode_type)
        {
        case I_COMMENTS:
            kdb_cmt.add(dup_obj_name, kdb_cmt.copy(obj_name));
            break;
        case I_EQUATIONS:
            throw IodeException("Cannot duplicate an equation");
            break;     
        case I_IDENTITIES:
            kdb_idt.add(dup_obj_name, kdb_idt.copy(obj_name));
            break; 
        case I_LISTS:
            kdb_lst.add(dup_obj_name, kdb_lst.copy(obj_name));
            break; 
        case I_SCALARS:
            kdb_scl.add(dup_obj_name, kdb_scl.copy(obj_name));
            break; 
        case I_TABLES:
            kdb_tbl.add(dup_obj_name, kdb_tbl.copy(obj_name));
            break; 
        case I_VARIABLES:
            kdb_var.add(dup_obj_name, kdb_var.copy(obj_name));
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

void QIodeMenuDataDuplicateObj::help()
{
	QDesktopServices::openUrl(url_manual);
}
