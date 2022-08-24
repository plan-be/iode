#include "compute_scc_decomposition.h"


/* NOTE FOR THE DEVELOPERS:
 * The constructor of a Menu Dialog class deriving from QIodeSettings MUST:
 * 2. call setupUi(this),
 * 3. use the Wrapper classes defined in the wrapper_classes.h header file to wrap the input field items,
 * 4. fill the mapFields member (inherited from QIodeSettings) with the wrapped field items,
 * 5. initialize the inherited className member (using typeid(this).name())
 * 6. call the loadSettings() method (inherited from QIodeSettings).
 */


QIodeMenuComputeSCCDecomposition::QIodeMenuComputeSCCDecomposition(const QString& settings_filepath, QWidget* parent, Qt::WindowFlags f) : 
    QIodeSettings(settings_filepath, parent, f)
{
    setupUi(this);

    qEquationsList = new WrapperQTextEdit(label_equations_list->text(), *textEdit_equations_list, OPTIONAL_FIELD);
    qTriangulationIterations = new WrapperSpinBox(label_triangulation_iterations->text(), *spinBox_triangulation_iterations, REQUIRED_FIELD);
    qPreRecursiveListName = new WrapperQLineEdit(label_pre_recursive->text(), *lineEdit_pre_recursive_list_name, REQUIRED_FIELD);
    qInterRecursiveListName = new WrapperQLineEdit(label_inter_recursive->text(), *lineEdit_inter_recursive_list_name, REQUIRED_FIELD);
    qPostRecursiveListName = new WrapperQLineEdit(label_post_recursive->text(), *lineEdit_post_recursive_list_name, REQUIRED_FIELD);

    mapFields["EquationsList"] = qEquationsList;
    mapFields["NbIterations"] = qTriangulationIterations;
    mapFields["PreRecursiveListName"] = qPreRecursiveListName;
    mapFields["InterRecursiveListName"] = qInterRecursiveListName;
    mapFields["PostRecursiveListName"] = qPostRecursiveListName;

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

QIodeMenuComputeSCCDecomposition::~QIodeMenuComputeSCCDecomposition()
{
    delete qEquationsList;
    delete qTriangulationIterations;
    delete qPreRecursiveListName;
    delete qInterRecursiveListName;
    delete qPostRecursiveListName;
}

void QIodeMenuComputeSCCDecomposition::compute()
{
    try
    {
        std::string equations_list = qEquationsList->extractAndVerify().toStdString();
        int nb_iterations = qTriangulationIterations->extractAndVerify();
        std::string pre_recursive_list_name = qPreRecursiveListName->extractAndVerify().toStdString();
        std::string inter_recursive_list_name = qInterRecursiveListName->extractAndVerify().toStdString();
        std::string post_recursive_list_name = qPostRecursiveListName->extractAndVerify().toStdString();

        Simulation sim;

        sim.model_calculate_SCC(nb_iterations, pre_recursive_list_name, inter_recursive_list_name, post_recursive_list_name, equations_list);

        this->accept();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
    }
}