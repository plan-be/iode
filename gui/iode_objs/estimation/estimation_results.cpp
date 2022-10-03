#include "estimation_results.h"


QIodeEstimationResults::QIodeEstimationResults(Estimation* est, QWidget* parent, Qt::WindowFlags f) :
    QDialog(parent, f), precision(6), est(est)
{
    setupUi(this);

    set_coefficients_tab();

    set_correlation_matrix_tab();

    NamedEquation equation = est->current_equation();
    set_tests_tab(equation.eq);
}

void QIodeEstimationResults::set_coefficients_tab()
{
    QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";
    tableView_coefs->setStyleSheet(stylesheet);

    KDBScalars* kdb_coefs = est->get_coefficients();
    // Warning: need to create a copy of kdb_coefs because passed kdb is deleted in 
    //          the QIodeTemplateTableModel destructor
    ScalarsModel* scalarsModel = new ScalarsModel(this, new KDBScalars(*kdb_coefs));
    tableView_coefs->setModel(scalarsModel);
}

void QIodeEstimationResults::set_correlation_matrix_tab()
{
    QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";
    tableView_corr_matrix->setStyleSheet(stylesheet);

    QVector<QString> q_coefs_names;
    KDBScalars* kdb_coefs = est->get_coefficients();
    for(const std::string& name : kdb_coefs->get_names()) q_coefs_names << QString::fromStdString(name);
    CorrelationMatrixModel* model = new CorrelationMatrixModel(q_coefs_names, est->get_correlation_matrix(), this);
    tableView_corr_matrix->setModel(model);
}

void QIodeEstimationResults::set_tests_tab(Equation& eq)
{
    QGridLayout* layout = new QGridLayout(tab_tests);
    std::array<float, EQS_NBTESTS> tests = eq.get_tests();
    for(int i=0; i < vEquationTests.size(); i++)
    {
        QLabel* label = new QLabel(QString::fromStdString(vEquationTests[i]));
        layout->addWidget(label, i, 0, Qt::AlignLeft);
        QLineEdit* line = new QLineEdit(QString::number(tests[i], 'g', precision));
        line->setReadOnly(true);
        layout->addWidget(line, i, 1, Qt::AlignLeft);
    }
}

// TODO : implement this SLOT
void QIodeEstimationResults::plot_yobs_yest()
{

}

// TODO : implement this SLOT
void QIodeEstimationResults::plot_residual()
{

}

// TODO : implement this SLOT
void QIodeEstimationResults::print_graphs()
{

}

// TODO : implement this SLOT
void QIodeEstimationResults::print_output()
{

}

void QIodeEstimationResults::help()
{
	QDesktopServices::openUrl(url_manual);
}
