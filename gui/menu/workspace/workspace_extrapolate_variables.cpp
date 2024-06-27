#include "workspace_extrapolate_variables.h"


MenuWorkspaceExtrapolateVariables::MenuWorkspaceExtrapolateVariables(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

    completer = new IodeCompleter(false, false, VARIABLES, textEdit_variables_list);
    textEdit_variables_list->setCompleter(completer);

    QList<QString> methodsList; 
    for(const std::string& method : v_simulation_initialization) methodsList << QString::fromStdString(method);

    wFrom = new WrapperSampleEdit(label_sample_from->text(), *sampleEdit_sample_from, REQUIRED_FIELD);
    wTo = new WrapperSampleEdit(label_sample_to->text(), *sampleEdit_sample_to, REQUIRED_FIELD);
    wMethod = new WrapperComboBox(label_method->text(), *comboBox_method, REQUIRED_FIELD, methodsList);
    wVarsList = new WrapperQPlainTextEdit(label_variables_list->text(), *textEdit_variables_list, OPTIONAL_FIELD);

    mapFields["From"] = wFrom;
    mapFields["To"] = wTo;
    mapFields["Method"] = wMethod;
    mapFields["VarsList"] = wVarsList;

    className = "MENU_WORKSPACE_EXTRAPOLATE_VARIABLES";
    loadSettings();
}

MenuWorkspaceExtrapolateVariables::~MenuWorkspaceExtrapolateVariables()
{
    delete wFrom;
    delete wTo;
    delete wMethod;
    delete wVarsList;

    delete completer;
}

void MenuWorkspaceExtrapolateVariables::extrapolate_variables()
{
    try
    {
        std::string s_from = wFrom->extractAndVerify().toStdString();
        std::string s_to = wTo->extractAndVerify().toStdString();
        VariablesInitialization method = (VariablesInitialization) wMethod->extractAndVerify();
        std::string vars_list = wVarsList->extractAndVerify().toStdString();

        Variables.extrapolate(method, s_from, s_to, vars_list);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    
    this->accept();
}

void MenuWorkspaceExtrapolateVariables::help()
{
	QDesktopServices::openUrl(url_manual);
}
