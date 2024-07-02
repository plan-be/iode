#pragma once

#include "plot.h"

#include <QString>
#include <QVector>


class PlotTableDialog : public PlotDialog
{
    Q_OBJECT

    ComputedTableGraph* gsample_graph;

public:
    PlotTableDialog(ComputedTableGraph* gsample_graph, TableGraphType chartType = TABLE_GRAPH_LINE, const bool logScale = false, 
        TableGraphGrid xTicks = TABLE_GRAPH_MAJOR, TableGraphGrid yTicks = TABLE_GRAPH_MAJOR, 
        QWidget* parent = nullptr);
    ~PlotTableDialog();

protected:
    QString defaultTitle()
    {
        return QString::fromStdString(gsample_graph->get_title());
    }
};