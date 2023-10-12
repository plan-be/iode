#include "workspace_describe.h"


MenuWorkspaceDescribe::MenuWorkspaceDescribe(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

    wCommentsDescription = new WrapperQLineEdit(label_comments->text(), *lineEdit_comments, OPTIONAL_FIELD);
    wEquationsDescription = new WrapperQLineEdit(label_equations->text(), *lineEdit_equations, OPTIONAL_FIELD);
    wIdentitiesDescription = new WrapperQLineEdit(label_identities->text(), *lineEdit_identities, OPTIONAL_FIELD);
    wListsDescription = new WrapperQLineEdit(label_lists->text(), *lineEdit_lists, OPTIONAL_FIELD);
    wScalarsDescription = new WrapperQLineEdit(label_scalars->text(), *lineEdit_scalars, OPTIONAL_FIELD);
    wTablesDescription = new WrapperQLineEdit(label_tables->text(), *lineEdit_tables, OPTIONAL_FIELD);
    wVariablesDescription = new WrapperQLineEdit(label_variables->text(), *lineEdit_variables, OPTIONAL_FIELD);

    wCommentsDescription->setQValue(QString::fromStdString(Comments.get_description()));
    wEquationsDescription->setQValue(QString::fromStdString(Equations.get_description()));
    wIdentitiesDescription->setQValue(QString::fromStdString(Identities.get_description()));
    wListsDescription->setQValue(QString::fromStdString(Lists.get_description()));
    wScalarsDescription->setQValue(QString::fromStdString(Scalars.get_description()));
    wTablesDescription->setQValue(QString::fromStdString(Tables.get_description()));
    wVariablesDescription->setQValue(QString::fromStdString(Variables.get_description()));

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
            Comments.set_description(comments_description);

        if (!equations_description.empty())
            Equations.set_description(equations_description);

        if (!identites_description.empty())
            Identities.set_description(identites_description);

        if (!lists_description.empty())
            Lists.set_description(lists_description);

        if (!scalars_description.empty())
            Scalars.set_description(scalars_description);

        if (!tables_description.empty())
            Tables.set_description(tables_description);

        if (!variables_description.empty())
            Variables.set_description(variables_description);
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
