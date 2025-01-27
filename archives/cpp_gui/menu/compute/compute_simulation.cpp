#include "compute_simulation.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from IodeSettingsDialog MUST:
 * 2. call setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from IodeSettingsDialog) with the wrapped field items,
 * 5. initialize the inherited className member
 * 6. call the loadSettings() method (inherited from IodeSettingsDialog).
 */


MenuComputeSimulation::MenuComputeSimulation(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

	completer_eqs = new IodeCompleter(false, false, EQUATIONS, textEdit_equations_list);
	textEdit_equations_list->setCompleter(completer_eqs);
	completer_var = new IodeCompleter(false, false, VARIABLES, textEdit_exchange);
	textEdit_exchange->setCompleter(completer_var);

    for(const std::string& initialisation: v_simulation_initialization) qInitialisationList << QString::fromStdString(initialisation);
    for(const std::string& sort_algo: v_simulation_sort_algorithm) qSortAlgoList << QString::fromStdString(sort_algo);

    qEquationsList = new WrapperQPlainTextEdit(label_equations_list->text(), *textEdit_equations_list, OPTIONAL_FIELD);
    qExchange = new WrapperQPlainTextEdit(label_exchange->text(), *textEdit_exchange, OPTIONAL_FIELD);
    qFrom = new WrapperSampleEdit(label_simulation_from->text(), *sampleEdit_sample_from, REQUIRED_FIELD);
    qTo = new WrapperSampleEdit(label_simulation_to->text(), *sampleEdit_sample_to, REQUIRED_FIELD);
    qConvergence = new WrapperQLineEdit(label_convergence->text(), *lineEdit_convergence, REQUIRED_FIELD);
    qMaxIterations = new WrapperSpinBox(label_max_iterations->text(), *spinBox_max_iterations, REQUIRED_FIELD);
    qCheckDebug = new WrapperCheckBox(label_debug->text(), *checkBox_debug, REQUIRED_FIELD);
    qRelaxation = new WrapperQLineEdit(label_relaxation->text(), *lineEdit_relaxation, REQUIRED_FIELD);
    qSimInitialisation = new WrapperComboBox(label_initialisation->text(), *comboBox_initialization, REQUIRED_FIELD, qInitialisationList);
    qSortAlgorithm = new WrapperComboBox(label_sort_algorithm->text(), *comboBox_sort_algorithm, REQUIRED_FIELD, qSortAlgoList);
    qNbPasses = new WrapperSpinBox(label_passes->text(), *spinBox_nb_passes, REQUIRED_FIELD);

    mapFields["EquationsList"] = qEquationsList;
    mapFields["Exchange"] = qExchange;
    mapFields["From"] = qFrom;
    mapFields["To"] = qTo;
    mapFields["Convergence"] = qConvergence;
    mapFields["MaxIterations"] = qMaxIterations;
    mapFields["CheckDebug"] = qCheckDebug;
    mapFields["Relaxation"] = qRelaxation;
    mapFields["SimInitialisation"] = qSimInitialisation;
    mapFields["SortAlgorithm"] = qSortAlgorithm;
    mapFields["NbPasses"] = qNbPasses;

    className = "MENU_COMPUTE_SIMULATION";
    loadSettings();
}

MenuComputeSimulation::~MenuComputeSimulation()
{
    delete qEquationsList;
    delete qExchange;
    delete qFrom;
    delete qTo;
    delete qConvergence;
    delete qMaxIterations;
    delete qCheckDebug;
    delete qRelaxation;
    delete qSimInitialisation;
    delete qSortAlgorithm;
    delete qNbPasses;

    delete completer_eqs;
    delete completer_var;
}

void MenuComputeSimulation::compute()
{
    try
    {
        std::string equations_list = qEquationsList->extractAndVerify().toStdString();
        std::string exchange = qExchange->extractAndVerify().toStdString();
        std::string from = qFrom->extractAndVerify().toStdString();
        std::string to = qTo->extractAndVerify().toStdString();
        double convergence = std::stod(qConvergence->extractAndVerify().toStdString());
        int max_iterations = qMaxIterations->extractAndVerify();
        bool debug = qCheckDebug->extractAndVerify();
        double relaxation = std::stod(qRelaxation->extractAndVerify().toStdString());
        VariablesInitialization initialisation = (VariablesInitialization) qSimInitialisation->extractAndVerify();
        SimuSortAlgorithm sort_algorithm = (SimuSortAlgorithm) qSortAlgorithm->extractAndVerify();
        int nb_passes = qNbPasses->extractAndVerify();

        Simulation sim;
        sim.set_convergence_threshold(convergence);
        sim.set_max_nb_iterations(max_iterations);
        sim.set_debug(debug);
        sim.set_relax(relaxation);
        sim.set_initialization_method(initialisation);
        sim.set_sort(sort_algorithm);
        sim.set_nb_passes(nb_passes);

        if(!exchange.empty()) sim.model_exchange(exchange);
        sim.model_simulate(from, to, equations_list);

        this->accept();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}