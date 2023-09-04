#pragma once

#include "plot.h"

#include <QString>
#include <QVector>


class PlotTableDialog : public PlotDialog
{
    Q_OBJECT

    GSampleGraph* gsample_graph;

public:
    PlotTableDialog(GSampleGraph* gsample_graph, EnumIodeGraphChart chartType = I_G_CHART_LINE, const bool logScale = false, 
        EnumIodeGraphAxisThicks xTicks = I_G_MAJOR_THICKS, EnumIodeGraphAxisThicks yTicks = I_G_MAJOR_THICKS, 
        QWidget* parent = nullptr);
    ~PlotTableDialog();

protected:
    QString defaultTitle()
    {
        return QString::fromStdString(gsample_graph->get_title());
    }
};