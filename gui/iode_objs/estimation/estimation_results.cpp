#include "estimation_results.h"


EstimationResultsDialog::EstimationResultsDialog(EditAndEstimateEquations* edit_est_eqs, QWidget* parent) :
    QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint), 
    precision(6), edit_est_eqs(edit_est_eqs)
{
    setupUi(this);
    stylesheetHeader = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";

    set_coefficients_tab();
    set_correlation_matrix_tab();
    set_tests_tab();

    Sample* sample = edit_est_eqs->get_sample();
    from = QString::fromStdString(sample->start_period().to_string());
    to = QString::fromStdString(sample->end_period().to_string());
    for(const std::string& name: edit_est_eqs->get_list_equations()) 
        variables_names << QString::fromStdString(name);

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
    tableView_coefs->setStyleSheet(stylesheetHeader);

    // NOTE: make a copy of the estimation scalars database because the database 
    //       passed to the constructor of ScalarsModel is deleted in its destructor 
    KDBScalars* kdb_scl = edit_est_eqs->get_scalars()->subset("*");
    ScalarsModel* scalarsModel = new ScalarsModel(this, kdb_scl);
    tableView_coefs->setModel(scalarsModel);
}

void EstimationResultsDialog::set_correlation_matrix_tab()
{
    tableView_corr_matrix->setStyleSheet(stylesheetHeader);

    try
    {
        CorrelationMatrixModel* model = new CorrelationMatrixModel(edit_est_eqs->get_correlation_matrix(), this);
        tableView_corr_matrix->setModel(model);
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", "Cannot display the correlation matrix.\n" + QString(e.what()));
    } 
}

void EstimationResultsDialog::set_tests_tab()
{
    tableView_tests->setStyleSheet(stylesheetHeader);

    try
    {
        TestsEqsModel* testsEqsModel = new TestsEqsModel(edit_est_eqs, this);
        tableView_tests->setModel(testsEqsModel);
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", "Cannot display the values for the tests.\n" + QString(e.what()));
    }
}

// same as o_estgr() in DOS o_gr.c + see ODE_blk_res_fn() case 6
void EstimationResultsDialog::plot_yobs_yest()
{
    Equation current_eq = edit_est_eqs->current_equation();
    std::string eq_name = current_eq.get_endo();

    std::pair<std::string, std::string> lrhs = current_eq.split_equation();
    QString lhs = QString::fromStdString(lrhs.first);

    // prepare local Variables KDB
    Variable values;
    Sample* sample = edit_est_eqs->get_sample();
    KDBVariables* kdb_vars = Variables.subset("", true);
    kdb_vars->set_sample(sample->start_period(), sample->end_period());

    PlotVariablesDialog* plotDialog = new PlotVariablesDialog(kdb_vars);

    // set title
    QString title = QString("Equation %1 : observed and fitted values").arg(QString::fromStdString(eq_name));
    plotDialog->setTitle(title);

    // set the periods for the plot
    plotDialog->setPeriods(*sample);

    // observed values
    values = edit_est_eqs->get_observed_values(eq_name);
    kdb_vars->add("OBSERVED", values);
    plotDialog->addSeries("OBSERVED", lhs + " : observed");

    // fitted values
    values = edit_est_eqs->get_fitted_values(eq_name);
    kdb_vars->add("FITTED", values);
    plotDialog->addSeries("FITTED", lhs + " : fitted");

    // set legend
    plotDialog->plot();
    emit newPlot(plotDialog);
}

// same as o_estgr() in DOS o_gr.c + see ODE_blk_res_fn() case 7
void EstimationResultsDialog::plot_residual()
{
    Equation current_eq = edit_est_eqs->current_equation();
    std::string eq_name = current_eq.get_endo();

    std::pair<std::string, std::string> lrhs = current_eq.split_equation();
    QString lhs = QString::fromStdString(lrhs.first);

    // prepare local Variables KDB
    Sample* sample = edit_est_eqs->get_sample();
    KDBVariables* kdb_vars = Variables.subset("", true);
    kdb_vars->set_sample(sample->start_period(), sample->end_period());

    PlotVariablesDialog* plotDialog = new PlotVariablesDialog(kdb_vars);

    // set title
    QString title = QString("Equation %1 : residuals").arg(QString::fromStdString(eq_name));
    plotDialog->setTitle(title);

    // set the periods for the plot
    plotDialog->setPeriods(*sample);

    // residual values
    Variable values = edit_est_eqs->get_residual_values(eq_name);
    kdb_vars->add("RESIDUALS", values);
    plotDialog->addSeries("RESIDUALS", lhs + " : residuals");

    plotDialog->plot();
    emit newPlot(plotDialog);
}

void EstimationResultsDialog::dump_table(const QAbstractTableModel* table_model, QTextCursor& cursor)
{
	QTextTableCell headerCell;
	QTextTableCell cell;
	QString text;
	QTextCharFormat boldFormat;
	boldFormat.setFontWeight(QFont::Bold);

    QTextTable* table = cursor.insertTable(table_model->rowCount() + 1, table_model->columnCount() + 1);
	QTextTableFormat tableFormat = table->format();
	tableFormat.setHeaderRowCount(1);
	tableFormat.setCellPadding(4);
	tableFormat.setCellSpacing(0);
	table->setFormat(tableFormat);

	// top left cell
	headerCell = table->cellAt(0, 0);
	QTextTableCellFormat cellFormat = headerCell.format().toTableCellFormat();
    cellFormat.setRightBorderStyle(QTextFrameFormat::BorderStyle_Solid);
	cellFormat.setBottomBorderStyle(QTextFrameFormat::BorderStyle_Solid);
	headerCell.setFormat(cellFormat);

    // columns header
    for(int col=0; col < table_model->columnCount(); col++)
    {
        text = table_model->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString();
        headerCell = table->cellAt(0, col + 1);
        cursor = headerCell.firstCursorPosition();
        cursor.insertText(text, boldFormat);
        QTextTableCellFormat cellFormat = headerCell.format().toTableCellFormat();
        cellFormat.setBottomBorderStyle(QTextFrameFormat::BorderStyle_Solid);
        headerCell.setFormat(cellFormat);
    }

    // table rows
    for(int row=0; row < table_model->rowCount(); row++)
    {
        // row header
        text = table_model->headerData(row, Qt::Vertical, Qt::DisplayRole).toString();
        headerCell = table->cellAt(row + 1, 0);
        cursor = headerCell.firstCursorPosition();
        cursor.insertText(text, boldFormat);
        QTextTableCellFormat cellFormat = headerCell.format().toTableCellFormat();
        cellFormat.setRightBorderStyle(QTextFrameFormat::BorderStyle_Solid);
        headerCell.setFormat(cellFormat);

        // row cells content
        for(int col=0; col < table_model->columnCount(); col++)
        {
            text = table_model->data(table_model->index(row, col), Qt::DisplayRole).toString();
            cell = table->cellAt(row + 1, col + 1);
            cursor = cell.firstCursorPosition();
            cursor.insertText(text);
        }
    }

    cursor.movePosition(QTextCursor::End);
}

void EstimationResultsDialog::print_output()
{
    QTextDocument document;
    QTextCursor cursor(&document);

    // ---- dump the coefficients matrix ----

    cursor.insertHtml("<h3>Coefficients</h3>");
    cursor.insertText("\n\n");

    QAbstractTableModel* scalars_model = static_cast<QAbstractTableModel*>(tableView_coefs->model());
    dump_table(scalars_model, cursor);
    cursor.insertText("\n\n");

    // ---- dump the correlation matrix ----

    cursor.insertHtml("<h3>Correlation Matrix</h3>");
    cursor.insertText("\n\n");

    QAbstractTableModel* coeffs_model = static_cast<QAbstractTableModel*>(tableView_corr_matrix->model());
    dump_table(coeffs_model, cursor);
    cursor.insertText("\n\n");

    // ---- dump the test results ----

    cursor.insertHtml("<h3>Tests By Equation</h3>");
    cursor.insertText("\n\n");

    Equation eq = edit_est_eqs->current_equation();
    std::array<float, EQS_NBTESTS> tests = eq.get_tests();

    QTextTable* table = cursor.insertTable(vEquationTests.size() + 1, 2);
	QTextTableFormat tableFormat = table->format();
	tableFormat.setHeaderRowCount(1);
	tableFormat.setCellPadding(4);
	tableFormat.setCellSpacing(0);
	table->setFormat(tableFormat);

	// columns header
	QTextTableCell headerCell = table->cellAt(0, 0);
	QTextTableCellFormat cellFormat = headerCell.format().toTableCellFormat();
    QTextCharFormat boldFormat;
	boldFormat.setFontWeight(QFont::Bold);
    cursor = headerCell.firstCursorPosition();
    cursor.insertText("Test Name", boldFormat);
	cellFormat.setBottomBorderStyle(QTextFrameFormat::BorderStyle_Solid);
	headerCell.setFormat(cellFormat);

    // columns header
    QStringList header_names = {"Test Name", "Value"};
    for(int col=0; col < header_names.size(); col++)
    {
        headerCell = table->cellAt(0, col);
        cursor = headerCell.firstCursorPosition();
        cursor.insertText(header_names[col], boldFormat);
        QTextTableCellFormat cellFormat = headerCell.format().toTableCellFormat();
        cellFormat.setBottomBorderStyle(QTextFrameFormat::BorderStyle_Solid);
        headerCell.setFormat(cellFormat);
    }

    // rows
    QTextTableCell cell;
    for(int row=0; row < vEquationTests.size(); row++)
    {
        cell = table->cellAt(row + 1, 0);
        cursor = cell.firstCursorPosition();
        cursor.insertText(QString::fromStdString(vEquationTests[row]));

        cell = table->cellAt(row + 1, 1);
        cursor = cell.firstCursorPosition();
        cursor.insertText(QString::number(tests[row], 'g', 8));
    }

    cursor.movePosition(QTextCursor::End);

    // ---- print ----

    QPrinter printer;
    QPrintPreviewDialog dialog(&printer);
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, [&](){ document.print(&printer); });
    dialog.exec();
}

void EstimationResultsDialog::help()
{
	QDesktopServices::openUrl(url_manual);
}
