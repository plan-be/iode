#include "plot_vars.h"

PlotVariablesDialog::PlotVariablesDialog(KDBVariables* kdb_vars, TableGraphType chartType, 
    IodeVarMode varMode, const bool logScale, TableGraphGrid xTicks, TableGraphGrid yTicks, 
    QWidget* parent) : PlotDialog(chartType, logScale, xTicks, yTicks, parent), varMode(varMode)
{
    if(kdb_vars)
        this->kdb_vars = kdb_vars;
    else
        this->kdb_vars = new KDBVariables();

    comboVarMode = new QComboBox();
    QStringList q_var_modes;
    for(const std::string& s_mode: v_var_modes) q_var_modes << QString::fromStdString(s_mode);
    comboVarMode->addItems(q_var_modes);
    connect(comboVarMode, &QComboBox::currentIndexChanged, this, &PlotVariablesDialog::updateVarMode);
    layout->addWidget(comboVarMode, 1, 1, Qt::AlignCenter);
}

PlotVariablesDialog::~PlotVariablesDialog()
{
    delete comboVarMode;

    delete kdb_vars;
}

void PlotVariablesDialog::addSeries(const QString& variable, const QString& legend_name)
{
    std::string var_name = variable.toStdString();
    QString series_name = (legend_name.isEmpty()) ? variable : legend_name;

    QVector<double> y;
    for(int t = start_t; t <= end_t; t++)
        y.append(kdb_vars->get_var(var_name, t, varMode));

    int series_pos = chart_series.size();
    QColor color = LINE_COLORS[series_pos % LINE_COLORS.size()];
    Qt::PenStyle style = LINE_STYLES[series_pos / LINE_COLORS.size()];

    chart_series.append(PlotSeries(series_name, y, color, style));
}

void PlotVariablesDialog::updateVarMode(int index)
{
    if(index < 0 || index >= IODE_NB_VAR_MODES)
        return;
    
    varMode = (IodeVarMode) index;

    for(PlotSeries& series: chart_series)
    {
        std::string var_name = series.name.toStdString();

        for(int t = start_t; t <= end_t; t++)
            series.y[t - start_t] = kdb_vars->get_var(var_name, t, varMode);
    }

    buildSeries();
}