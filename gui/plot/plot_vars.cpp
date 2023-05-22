#include "plot_vars.h"

QIodePlotVariablesDialog::QIodePlotVariablesDialog(KDBVariables* kdb_vars, EnumIodeGraphChart chartType, 
    EnumIodeGraphAxisType axisType, const bool logScale, EnumIodeGraphAxisThicks xTicks, EnumIodeGraphAxisThicks yTicks, 
    QWidget* parent, Qt::WindowFlags f) : QIodePlotDialog(chartType, logScale, xTicks, yTicks, parent, f), axisType(axisType)
{
    if(kdb_vars)
        this->kdb_vars = kdb_vars;
    else
        this->kdb_vars = new KDBVariables();

    comboXAxisType = new QComboBox();
    QStringList q_axis_types;
    for(const std::string& axis_type: vGraphsXAxisTypes) q_axis_types << QString::fromStdString(axis_type);
    comboXAxisType->addItems(q_axis_types);
    connect(comboXAxisType, &QComboBox::currentIndexChanged, this, &QIodePlotVariablesDialog::updateXAxisType);
    layout->addWidget(comboXAxisType, 1, 1, Qt::AlignCenter);
}

QIodePlotVariablesDialog::~QIodePlotVariablesDialog()
{
    delete comboXAxisType;

    delete kdb_vars;
}

void QIodePlotVariablesDialog::addSeries(const QString& variable, const QString& legend_name)
{
    std::string var_name = variable.toStdString();

    QVector<double> values;
    for(int t = start_t; t <= end_t; t++)
        values.append(kdb_vars->get_var(var_name, t, axisType));
    chart_series[variable] = values;

    legend_series[variable] = (legend_name.isEmpty()) ? variable : legend_name;
}

void QIodePlotVariablesDialog::updateXAxisType(int index)
{
    if(index < 0 || index >= I_NB_X_AXIS_TYPES)
        return;
    
    axisType = (EnumIodeGraphAxisType) index;

    QMap<QString, QVector<double>>::iterator it = chart_series.begin();
    while (it != chart_series.end())
    {
        std::string var_name = it.key().toStdString();
        int i=0;
        for(int t = start_t; t <= end_t; t++)
            it.value()[i++] = kdb_vars->get_var(var_name, t, axisType);
        ++it;
    }

    buildSeries();
}