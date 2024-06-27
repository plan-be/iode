#include "compute_model.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from IodeSettingsDialog MUST:
 * 2. call setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from IodeSettingsDialog) with the wrapped field items,
 * 5. initialize the inherited className member
 * 6. call the loadSettings() method (inherited from IodeSettingsDialog).
 */


MenuComputeModel::MenuComputeModel(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

	completer = new IodeCompleter(false, false, EQUATIONS, textEdit_equations_list);
	textEdit_equations_list->setCompleter(completer);

    qEquationsList = new WrapperQPlainTextEdit(label_equations_list->text(), *textEdit_equations_list, OPTIONAL_FIELD);
    mapFields["EquationsList"] = qEquationsList;

    className = "MENU_COMPUTE_MODEL";
    loadSettings();
}

MenuComputeModel::~MenuComputeModel()
{
    delete qEquationsList;
    delete completer;
}

void MenuComputeModel::compute()
{
    try
    {
        std::string equations_list = qEquationsList->extractAndVerify().toStdString();

        Simulation sim;

        sim.model_compile(equations_list);

        this->accept();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}