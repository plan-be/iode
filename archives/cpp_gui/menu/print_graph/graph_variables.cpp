#include "graph_variables.h"


MenuGraphVariables::MenuGraphVariables(QWidget* parent) : 
    IodeSettingsDialog(parent)
{
    setupUi(this);

    completer = new IodeCompleter(false, false, VARIABLES, textEdit_variables);
    textEdit_variables->setCompleter(completer);

    lineEdit_min_Y->setValidator(new QDoubleValidator(lineEdit_min_Y));
    lineEdit_max_Y->setValidator(new QDoubleValidator(lineEdit_max_Y));

    QList<QString> q_var_modes;
    for(const std::string& var_mode: v_var_modes) q_var_modes << QString::fromStdString(var_mode);
    QList<QString> q_chart_types;
    for(const std::string& chart_type: v_graph_chart_types) q_chart_types << QString::fromStdString(chart_type);
    QList<QString> q_axis_ticks;
    for(const std::string& axis_ticks: v_graph_axis_thicks) q_axis_ticks << QString::fromStdString(axis_ticks);
    QList<QString> q_langs;
    for(const std::string& lang: v_table_langs) q_langs << QString::fromStdString(lang);

    wVariables = new WrapperQPlainTextEdit(label_variables->text(), *textEdit_variables, REQUIRED_FIELD);
    wVarMode = new WrapperComboBox(label_x_axis_type->text(), *comboBox_x_axis_type, REQUIRED_FIELD, q_var_modes);
    wFrom = new WrapperSampleEdit(label_sample_from->text(), *sampleEdit_sample_from, REQUIRED_FIELD);
    wTo = new WrapperSampleEdit(label_sample_to->text(), *sampleEdit_sample_to, REQUIRED_FIELD);
    wChartType = new WrapperComboBox(label_chart_type->text(), *comboBox_chart_type, REQUIRED_FIELD, q_chart_types);
    wYAxisScale = new WrapperCheckBox(label_chart_type->text(), *checkBox_log_scale, REQUIRED_FIELD);
    wXTicks = new WrapperComboBox(label_X_ticks->text(), *comboBox_X_ticks, REQUIRED_FIELD, q_axis_ticks);
    wYTicks = new WrapperComboBox(label_Y_ticks->text(), *comboBox_Y_ticks, REQUIRED_FIELD, q_axis_ticks);
    wMinY = new WrapperQLineEdit(label_min_Y->text(), *lineEdit_min_Y, OPTIONAL_FIELD);
    wMaxY = new WrapperQLineEdit(label_max_Y->text(), *lineEdit_max_Y, OPTIONAL_FIELD);
    wLanguage = new WrapperComboBox(label_language->text(), *comboBox_language, REQUIRED_FIELD, q_langs);
    
    mapFields["Variables"]  = wVariables;
    mapFields["VarMode"]    = wVarMode;
    mapFields["From"]       = wFrom;
    mapFields["To"]         = wTo;
    mapFields["ChartType"]  = wChartType;
    mapFields["YAxisScale"] = wYAxisScale;
    mapFields["XTicks"]     = wXTicks;
    mapFields["YTicks"]     = wYTicks;
    mapFields["MinY"]       = wMinY;
    mapFields["MaxY"]       = wMaxY;
    mapFields["Language"]   = wLanguage;

    className = "MENU_GRAPH_VARIABLES";
    loadSettings();
}

MenuGraphVariables::~MenuGraphVariables()
{
    delete wVariables;
    delete wVarMode;
    delete wFrom;
    delete wTo;
    delete wChartType;
    delete wYAxisScale;
    delete wXTicks;
    delete wYTicks;
    delete wMinY;
    delete wMaxY;
    delete wLanguage;

    delete completer;
}

void MenuGraphVariables::display()
{
    try
    {
        TableGraphType chartType = (TableGraphType) wChartType->extractAndVerify();
        IodeVarMode varMode = (IodeVarMode) wVarMode->extractAndVerify();
        bool logScale = wYAxisScale->extractAndVerify();
        TableGraphGrid xTicks = (TableGraphGrid) wXTicks->extractAndVerify();
        TableGraphGrid yTicks = (TableGraphGrid) wYTicks->extractAndVerify();

        PlotVariablesDialog* plotDialog = new PlotVariablesDialog(nullptr, chartType, varMode, logScale, xTicks, yTicks);

        // periods for the plot
        QString from = wFrom->extractAndVerify();
        QString to = wTo->extractAndVerify();
        plotDialog->setPeriods(*Variables.get_sample(), from, to);

        // set min and max Y
        QString qMinY = wMinY->extractAndVerify();
        if(!qMinY.isEmpty())
            plotDialog->setMinValue(qMinY.toDouble());

        QString qMaxY = wMaxY->extractAndVerify();
        if(!qMaxY.isEmpty())
            plotDialog->setMaxValue(qMaxY.toDouble());

        // add plot series
        std::string pattern = wVariables->extractAndVerify().toStdString();
        std::vector<std::string> list_vars_names = Variables.get_names(pattern);
        for(const std::string var_name: list_vars_names)
        {
            QString variable = QString::fromStdString(var_name);
            plotDialog->addSeries(variable);
        }
        
        // not used
        TableLang lang = (TableLang) wLanguage->extractAndVerify();

        // build the plot
        plotDialog->plot();
        emit newPlot(plotDialog);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    
    this->accept();
}

void MenuGraphVariables::apply()
{
    try
    {
        QMessageBox::warning(this, "WARNING", "Apply is not yet implemented");
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
}

void MenuGraphVariables::setup()
{
    try
    {
        MenuFileSettings dialog(this);
        dialog.exec();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
}

void MenuGraphVariables::help()
{
	
	QDesktopServices::openUrl(url_manual);
}
