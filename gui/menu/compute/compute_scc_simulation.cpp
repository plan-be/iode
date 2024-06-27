#include "compute_scc_simulation.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from IodeSettingsDialog MUST:
 * 2. call setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from IodeSettingsDialog) with the wrapped field items,
 * 5. initialize the inherited className member
 * 6. call the loadSettings() method (inherited from IodeSettingsDialog).
 */


MenuComputeSCCSimulation::MenuComputeSCCSimulation(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

    for(const std::string& initialisation: v_simulation_initialization) qInitialisationList << QString::fromStdString(initialisation);

    qFrom = new WrapperSampleEdit(label_simulation_from->text(), *sampleEdit_sample_from, REQUIRED_FIELD);
    qTo = new WrapperSampleEdit(label_simulation_to->text(), *sampleEdit_sample_to, REQUIRED_FIELD);
    qConvergence = new WrapperQLineEdit(label_convergence->text(), *lineEdit_convergence, REQUIRED_FIELD);
    qMaxIterations = new WrapperSpinBox(label_max_iterations->text(), *spinBox_max_iterations, REQUIRED_FIELD);
    qCheckDebug = new WrapperCheckBox(label_debug->text(), *checkBox_debug, REQUIRED_FIELD);
    qRelaxation = new WrapperQLineEdit(label_relaxation->text(), *lineEdit_relaxation, REQUIRED_FIELD);
    qSimInitialisation = new WrapperComboBox(label_initialisation->text(), *comboBox_initialization, REQUIRED_FIELD, qInitialisationList);
    qPreRecursiveListName = new WrapperQLineEdit(label_pre_recursive->text(), *lineEdit_pre_recursive_list_name, REQUIRED_FIELD);
    qInterRecursiveListName = new WrapperQLineEdit(label_inter_recursive->text(), *lineEdit_inter_recursive_list_name, REQUIRED_FIELD);
    qPostRecursiveListName = new WrapperQLineEdit(label_post_recursive->text(), *lineEdit_post_recursive_list_name, REQUIRED_FIELD);

    mapFields["From"] = qFrom;
    mapFields["To"] = qTo;
    mapFields["Convergence"] = qConvergence;
    mapFields["MaxIterations"] = qMaxIterations;
    mapFields["CheckDebug"] = qCheckDebug;
    mapFields["Relaxation"] = qRelaxation;
    mapFields["SimInitialisation"] = qSimInitialisation;
    mapFields["PreRecursiveListName"] = qPreRecursiveListName;
    mapFields["InterRecursiveListName"] = qInterRecursiveListName;
    mapFields["PostRecursiveListName"] = qPostRecursiveListName;

    className = "MENU_COMPUTE_SCC_SIMULATION";
    loadSettings();
}

MenuComputeSCCSimulation::~MenuComputeSCCSimulation()
{
    delete qFrom;
    delete qTo;
    delete qConvergence;
    delete qMaxIterations;
    delete qCheckDebug;
    delete qRelaxation;
    delete qSimInitialisation;
    delete qPreRecursiveListName;
    delete qInterRecursiveListName;
    delete qPostRecursiveListName;
}

void MenuComputeSCCSimulation::compute()
{
    try
    {
        std::string from = qFrom->extractAndVerify().toStdString();
        std::string to = qTo->extractAndVerify().toStdString();
        double convergence = std::stod(qConvergence->extractAndVerify().toStdString());
        int max_iterations = qMaxIterations->extractAndVerify();
        bool debug = qCheckDebug->extractAndVerify();
        double relaxation = std::stod(qRelaxation->extractAndVerify().toStdString());
        VariablesInitialization initialisation = (VariablesInitialization) qSimInitialisation->extractAndVerify();
        std::string pre_recursive_list_name = qPreRecursiveListName->extractAndVerify().toStdString();
        std::string inter_recursive_list_name = qInterRecursiveListName->extractAndVerify().toStdString();
        std::string post_recursive_list_name = qPostRecursiveListName->extractAndVerify().toStdString();

        Simulation sim;
        sim.set_convergence_threshold(convergence);
        sim.set_max_nb_iterations(max_iterations);
        sim.set_debug(debug);
        sim.set_relax(relaxation);
        sim.set_initialization_method(initialisation);

        sim.model_simulate_SCC(from, to, pre_recursive_list_name, inter_recursive_list_name, post_recursive_list_name);

        this->accept();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}