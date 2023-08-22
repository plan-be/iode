#include "plot_vars.h"

QIodePlotVariablesDialog::QIodePlotVariablesDialog(KDBVariables* kdb_vars, EnumIodeGraphChart chartType, 
    EnumIodeVarMode varMode, const bool logScale, EnumIodeGraphAxisThicks xTicks, EnumIodeGraphAxisThicks yTicks, 
    QWidget* parent) : QIodePlotDialog(chartType, logScale, xTicks, yTicks, parent), varMode(varMode)
{
    if(kdb_vars)
        this->kdb_vars = kdb_vars;
    else
        this->kdb_vars = new KDBVariables();

    comboVarMode = new QComboBox();
    QStringList q_var_modes;
    for(const std::string& s_mode: v_var_modes) q_var_modes << QString::fromStdString(s_mode);
    comboVarMode->addItems(q_var_modes);
    connect(comboVarMode, &QComboBox::currentIndexChanged, this, &QIodePlotVariablesDialog::updateVarMode);
    layout->addWidget(comboVarMode, 1, 1, Qt::AlignCenter);
}

QIodePlotVariablesDialog::~QIodePlotVariablesDialog()
{
    delete comboVarMode;

    delete kdb_vars;
}

void QIodePlotVariablesDialog::addSeries(const QString& variable, const QString& legend_name)
{
    std::string var_name = variable.toStdString();

    QVector<double> values;
    for(int t = start_t; t <= end_t; t++)
        values.append(kdb_vars->get_var(var_name, t, varMode));
    chart_series[variable] = values;

    legend_series[variable] = (legend_name.isEmpty()) ? variable : legend_name;
}

void QIodePlotVariablesDialog::updateVarMode(int index)
{
    if(index < 0 || index >= I_VAR_NB_MODES)
        return;
    
    varMode = (EnumIodeVarMode) index;

    QMap<QString, QVector<double>>::iterator it = chart_series.begin();
    while (it != chart_series.end())
    {
        std::string var_name = it.key().toStdString();
        int i=0;
        for(int t = start_t; t <= end_t; t++)
            it.value()[i++] = kdb_vars->get_var(var_name, t, varMode);
        ++it;
    }

    buildSeries();
}