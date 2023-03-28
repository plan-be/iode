#include "graph_variables.h"


QIodeMenuGraphVariables::QIodeMenuGraphVariables(const QString& settings_filepath, QWidget* parent, Qt::WindowFlags f) : 
    QIodeSettings(settings_filepath, parent, f)
{
    setupUi(this);

    lineEdit_min_Y->setValidator(new QDoubleValidator(lineEdit_min_Y));
    lineEdit_max_Y->setValidator(new QDoubleValidator(lineEdit_max_Y));

    QList<QString> q_axis_types;
    for(const std::string& axis_type: vGraphsXAxisTypes) q_axis_types << QString::fromStdString(axis_type);
    QList<QString> q_chart_types;
    for(const std::string& chart_type: vGraphsChartTypes) q_chart_types << QString::fromStdString(chart_type);
    QList<QString> q_axis_ticks;
    for(const std::string& axis_ticks: vGraphsAxisThicks) q_axis_ticks << QString::fromStdString(axis_ticks);
    QList<QString> q_langs;
    for(const std::string& lang: vLangs) q_langs << QString::fromStdString(lang);

    wVariables = new WrapperQTextEdit(label_variables->text(), *textEdit_variables, REQUIRED_FIELD);
    wXAxisType = new WrapperComboBox(label_x_axis_type->text(), *comboBox_x_axis_type, REQUIRED_FIELD, q_axis_types);
    wFrom = new WrapperSampleEdit(label_sample_from->text(), *sampleEdit_sample_from, REQUIRED_FIELD);
    wTo = new WrapperSampleEdit(label_sample_to->text(), *sampleEdit_sample_to, REQUIRED_FIELD);
    wChartType = new WrapperComboBox(label_chart_type->text(), *comboBox_chart_type, REQUIRED_FIELD, q_chart_types);
    wYAxisScale = new WrapperCheckBox(label_chart_type->text(), *checkBox_log_scale, REQUIRED_FIELD);
    wXTicks = new WrapperComboBox(label_X_ticks->text(), *comboBox_X_ticks, REQUIRED_FIELD, q_axis_ticks);
    wYTicks = new WrapperComboBox(label_Y_ticks->text(), *comboBox_Y_ticks, REQUIRED_FIELD, q_axis_ticks);
    wMinY = new WrapperQLineEdit(label_min_Y->text(), *lineEdit_min_Y, OPTIONAL_FIELD);
    wMaxY = new WrapperQLineEdit(label_max_Y->text(), *lineEdit_max_Y, OPTIONAL_FIELD);
    wLanguage = new WrapperComboBox(label_language->text(), *comboBox_language, REQUIRED_FIELD, q_langs);
    
    mapFields["Variables"] = wVariables;
    mapFields["XAxisType"] = wXAxisType;
    mapFields["From"]      = wFrom;
    mapFields["To"]        = wTo;
    mapFields["ChartType"] = wChartType;
    mapFields["YAxisScale"] = wYAxisScale;
    mapFields["XTicks"]   = wXTicks;
    mapFields["YTicks"]   = wYTicks;
    mapFields["MinY"]      = wMinY;
    mapFields["MaxY"]      = wMaxY;
    mapFields["Language"]  = wLanguage;

    // TODO: if possible, find a way to initialize className inside MixingSettings
    // NOTE FOR DEVELOPPERS: we cannot simply call the line below from the constructor of MixingSettings 
    //                       since in that case this refers to MixingSettings and NOT the derived class
    className = QString::fromStdString(typeid(this).name());
    loadSettings();
}

QIodeMenuGraphVariables::~QIodeMenuGraphVariables()
{
    delete wVariables;
    delete wXAxisType;
    delete wFrom;
    delete wTo;
    delete wChartType;
    delete wYAxisScale;
    delete wXTicks;
    delete wYTicks;
    delete wMinY;
    delete wMaxY;
    delete wLanguage;
}

void QIodeMenuGraphVariables::display()
{
    try
    {
        std::string vars_names = wVariables->extractAndVerify().toStdString();
        char** list_vars_names = filter_kdb_names(I_VARIABLES, vars_names);
        int nb_vars = SCR_tbl_size((unsigned char**) list_vars_names);
        QList<QString> qVarsList;
        for(int i=0; i<nb_vars; i++) qVarsList << QString::fromStdString(std::string(list_vars_names[i]));

        EnumIodeGraphAxisType axisType = (EnumIodeGraphAxisType) wXAxisType->extractAndVerify();
        QString from = wFrom->extractAndVerify();
        QString to = wTo->extractAndVerify();
        EnumIodeGraphChart chartType = (EnumIodeGraphChart) wChartType->extractAndVerify();
        bool logScale = wYAxisScale->extractAndVerify();
        EnumIodeGraphAxisThicks xTicks = (EnumIodeGraphAxisThicks) wXTicks->extractAndVerify();
        EnumIodeGraphAxisThicks yTicks = (EnumIodeGraphAxisThicks) wYTicks->extractAndVerify();
        QString qMinY = wMinY->extractAndVerify();
        double minY = qMinY.isEmpty() ? NAN : qMinY.toDouble();
        QString qMaxY = wMaxY->extractAndVerify();
        double maxY = qMaxY.isEmpty() ? NAN : qMaxY.toDouble();
        EnumLang lang = (EnumLang) wLanguage->extractAndVerify();

        // check that the ending period is after the starting period
        Period start_period(from.toStdString());
        Period end_period(to.toStdString());
        int diff_periods = end_period.difference(start_period);
        if(diff_periods <= 0)
        {
            QString msg = "Cannot create a plot since the starting period " + from + " is ";
            msg += (diff_periods == 0) ? " equal to " : "after ";
            msg += "the ending period " + to;
            QMessageBox::critical(this, tr("ERROR"), msg);
            return;
        }

        QIodePlotDialog* plotDialog = new QIodePlotDialog();
        plotDialog->plot(qVarsList, from, to, "", chartType, axisType, logScale, xTicks, yTicks, minY, maxY);
        emit newPlot(plotDialog);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
    
    this->accept();
}

void QIodeMenuGraphVariables::apply()
{
    try
    {

    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
}

void QIodeMenuGraphVariables::setup()
{
    try
    {

    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, tr("ERROR"), tr(e.what()));
        return;
    }
}

void QIodeMenuGraphVariables::help()
{
	
	QDesktopServices::openUrl(url_manual);
}
