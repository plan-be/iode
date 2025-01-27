#include "plot_table.h"


PlotTableDialog::PlotTableDialog(ComputedTableGraph* gsample_graph, TableGraphType chartType, const bool logScale, 
    TableGraphGrid xTicks, TableGraphGrid yTicks, QWidget* parent)
    : PlotDialog(chartType, logScale, xTicks, yTicks, parent), gsample_graph(gsample_graph)
{
    Sample* sample = gsample_graph->get_sample();
    setPeriods(*sample);

    QString name;
    QColor color;
    for(int fileop = 0; fileop < gsample_graph->get_nb_files_ops(); fileop++)
    {
        Qt::PenStyle lineStyle = LINE_STYLES[fileop % LINE_STYLES.size()];

        int i = 0;
        for(const auto& [key, y] : gsample_graph->get_series(fileop))
        {
            name = QString::fromStdString(key);
            QVector<double> y_(y.begin(), y.end());
            color = LINE_COLORS[i % LINE_COLORS.size()];
            chart_series.append(PlotSeries(name, y_, color, lineStyle));
            i++;
        }
    }
}

PlotTableDialog::~PlotTableDialog()
{
}
