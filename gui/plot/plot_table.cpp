#include "plot_table.h"


PlotTableDialog::PlotTableDialog(GSampleGraph* gsample_graph, EnumIodeGraphChart chartType, const bool logScale, 
    EnumIodeGraphAxisThicks xTicks, EnumIodeGraphAxisThicks yTicks, QWidget* parent)
    : PlotDialog(chartType, logScale, xTicks, yTicks, parent), gsample_graph(gsample_graph)
{
    Sample* sample = gsample_graph->get_sample();
    setPeriods(*sample);

    QString name;
    for(const auto& [key, y] : gsample_graph->get_series())
    {
        name = QString::fromStdString(key);
        legend_series[name] = name;
        chart_series[name] = QVector<double>(y.begin(), y.end());
    }
}

PlotTableDialog::~PlotTableDialog()
{
}
