#include "workspace_describe.h"


MenuWorkspaceDescribe::MenuWorkspaceDescribe(QWidget* parent) : 
    QIodeSettings(parent)
{
    setupUi(this);

    QList<QString> qIodeTypes;
    for(const std::string& iode_type: vIodeTypes) qIodeTypes << QString::fromStdString(iode_type);

    wCommentsDescription = new WrapperQLineEdit(label_comments->text(), *lineEdit_comments, OPTIONAL_FIELD);
    wEquationsDescription = new WrapperQLineEdit(label_equations->text(), *lineEdit_equations, OPTIONAL_FIELD);
    wIdentitiesDescription = new WrapperQLineEdit(label_identities->text(), *lineEdit_identities, OPTIONAL_FIELD);
    wListsDescription = new WrapperQLineEdit(label_lists->text(), *lineEdit_lists, OPTIONAL_FIELD);
    wScalarsDescription = new WrapperQLineEdit(label_scalars->text(), *lineEdit_scalars, OPTIONAL_FIELD);
    wTablesDescription = new WrapperQLineEdit(label_tables->text(), *lineEdit_tables, OPTIONAL_FIELD);
    wVariablesDescription = new WrapperQLineEdit(label_variables->text(), *lineEdit_variables, OPTIONAL_FIELD);

    wCommentsDescription->setQValue(QString::fromStdString(KDBComments().get_description()));
    wEquationsDescription->setQValue(QString::fromStdString(KDBEquations().get_description()));
    wIdentitiesDescription->setQValue(QString::fromStdString(KDBIdentities().get_description()));
    wListsDescription->setQValue(QString::fromStdString(KDBLists().get_description()));
    wScalarsDescription->setQValue(QString::fromStdString(KDBScalars().get_description()));
    wTablesDescription->setQValue(QString::fromStdString(KDBTables().get_description()));
    wVariablesDescription->setQValue(QString::fromStdString(KDBVariables().get_description()));

    className = "MENU_WORKSPACE_DESCRIBE";
    loadSettings();
}

MenuWorkspaceDescribe::~MenuWorkspaceDescribe()
{
    delete wCommentsDescription;
    delete wEquationsDescription;
    delete wIdentitiesDescription;
    delete wListsDescription;
    delete wScalarsDescription;
    delete wTablesDescription;
    delete wVariablesDescription;
}

void MenuWorkspaceDescribe::update_descriptions()
{
    try
    {
        std::string comments_description = wCommentsDescription->extractAndVerify().toStdString();
        std::string equations_description = wEquationsDescription->extractAndVerify().toStdString();
        std::string identites_description = wIdentitiesDescription->extractAndVerify().toStdString();
        std::string lists_description = wListsDescription->extractAndVerify().toStdString();
        std::string scalars_description = wScalarsDescription->extractAndVerify().toStdString();
        std::string tables_description = wTablesDescription->extractAndVerify().toStdString();
        std::string variables_description = wVariablesDescription->extractAndVerify().toStdString();

        if (!comments_description.empty())
        {
            KDBComments kdb;
            kdb.set_description(comments_description);
        }

        if (!equations_description.empty())
        {
            KDBEquations kdb;
            kdb.set_description(equations_description);
        }

        if (!identites_description.empty())
        {
            KDBIdentities kdb;
            kdb.set_description(identites_description);
        }

        if (!lists_description.empty())
        {
            KDBLists kdb;
            kdb.set_description(lists_description);
        }

        if (!scalars_description.empty())
        {
            KDBScalars kdb;
            kdb.set_description(scalars_description);
        }

        if (!tables_description.empty())
        {
            KDBTables kdb;
            kdb.set_description(tables_description);
        }

        if (!variables_description.empty())
        {
            KDBVariables kdb;
            kdb.set_description(variables_description);
        }
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }

    this->accept();
}

void MenuWorkspaceDescribe::help()
{
	QDesktopServices::openUrl(url_manual);
}
