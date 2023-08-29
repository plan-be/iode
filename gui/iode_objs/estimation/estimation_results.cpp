#include "estimation_results.h"


EstimationResultsDialog::EstimationResultsDialog(Estimation* est, QWidget* parent) :
    QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint), precision(6), est(est)
{
    setupUi(this);

    set_coefficients_tab();

    set_correlation_matrix_tab();

    NamedEquation equation = est->current_equation();
    set_tests_tab(equation.eq);

    Sample* sample = est->get_sample();
    from = QString::fromStdString(sample->start_period().to_string());
    to = QString::fromStdString(sample->end_period().to_string());
    for(const std::string& name: est->get_list_equations()) variables_names << QString::fromStdString(name);

	MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
	connect(this, &EstimationResultsDialog::newPlot, main_window, &MainWindowAbstract::appendDialog);

    fullScreenShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_X), this);
    fullScreenShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
    
    connect(fullScreenShortcut, &QShortcut::activated, this, &EstimationResultsDialog::showMaximized);
}

EstimationResultsDialog::~EstimationResultsDialog()
{
    delete fullScreenShortcut;
}

void EstimationResultsDialog::set_coefficients_tab()
{
    QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";
    tableView_coefs->setStyleSheet(stylesheet);

    KDBScalars* kdb_coefs = est->get_coefficients();
    // Warning: need to create a copy of kdb_coefs because passed kdb is deleted in 
    //          the IodeTemplateTableModel destructor
    ScalarsModel* scalarsModel = new ScalarsModel(this, new KDBScalars(*kdb_coefs));
    tableView_coefs->setModel(scalarsModel);
}

void EstimationResultsDialog::set_correlation_matrix_tab()
{
    QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";
    tableView_corr_matrix->setStyleSheet(stylesheet);

    QVector<QString> q_coefs_names;
    KDBScalars* kdb_coefs = est->get_coefficients();
    for(const std::string& name : kdb_coefs->get_names()) q_coefs_names << QString::fromStdString(name);
    CorrelationMatrixModel* model = new CorrelationMatrixModel(q_coefs_names, est->get_correlation_matrix(), this);
    tableView_corr_matrix->setModel(model);
}

void EstimationResultsDialog::set_tests_tab(Equation& eq)
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

// same as o_estgr() in DOS o_gr.c + see ODE_blk_res_fn() case 6
void EstimationResultsDialog::plot_yobs_yest()
{
    NamedEquation nEq = est->current_equation();
    std::pair<std::string, std::string> lrhs = nEq.eq.split_equation();
    QString lhs = QString::fromStdString(lrhs.first);

    // prepare local Variables KDB
    Variable values;
    Sample* sample = est->get_sample();
    KDBVariables* kdb_vars = new KDBVariables(KDB_LOCAL, "");
    kdb_vars->set_sample(sample->start_period(), sample->end_period());

    PlotVariablesDialog* plotDialog = new PlotVariablesDialog(kdb_vars);

    // set title
    QString title = QString("Equation %1 : observed and fitted values").arg(QString::fromStdString(nEq.name));
    plotDialog->setTitle(title);

    // set the periods for the plot
    plotDialog->setPeriods(*sample);

    // observed values
    values = est->get_observed_values(nEq.name);
    kdb_vars->add("OBSERVED", values);
    plotDialog->addSeries("OBSERVED", lhs + " : observed");

    // fitted values
    values = est->get_fitted_values(nEq.name);
    kdb_vars->add("FITTED", values);
    plotDialog->addSeries("FITTED", lhs + " : fitted");

    // set legend
    plotDialog->plot();
    emit newPlot(plotDialog);
}

// same as o_estgr() in DOS o_gr.c + see ODE_blk_res_fn() case 7
void EstimationResultsDialog::plot_residual()
{
    NamedEquation nEq = est->current_equation();
    std::pair<std::string, std::string> lrhs = nEq.eq.split_equation();
    QString lhs = QString::fromStdString(lrhs.first);

    // prepare local Variables KDB
    Sample* sample = est->get_sample();
    KDBVariables* kdb_vars = new KDBVariables(KDB_LOCAL, "");
    kdb_vars->set_sample(sample->start_period(), sample->end_period());

    PlotVariablesDialog* plotDialog = new PlotVariablesDialog(kdb_vars);

    // set title
    QString title = QString("Equation %1 : residuals").arg(QString::fromStdString(nEq.name));
    plotDialog->setTitle(title);

    // set the periods for the plot
    plotDialog->setPeriods(*sample);

    // residual values
    Variable values = est->get_residual_values(nEq.name);
    kdb_vars->add("RESIDUALS", values);
    plotDialog->addSeries("RESIDUALS", lhs + " : residuals");

    plotDialog->plot();
    emit newPlot(plotDialog);
}

// TODO : implement this SLOT
void EstimationResultsDialog::print_graphs()
{
    QMessageBox::warning(nullptr, "WARNING", "Not yet implemented");
}

// TODO : implement this SLOT
void EstimationResultsDialog::print_output()
{
    QMessageBox::warning(nullptr, "WARNING", "Not yet implemented");
}

void EstimationResultsDialog::help()
{
	QDesktopServices::openUrl(url_manual);
}
