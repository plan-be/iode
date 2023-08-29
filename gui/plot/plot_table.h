#pragma once

#include "plot.h"

#include <QString>
#include <QVector>


class PlotTableDialog : public PlotDialog
{
    Q_OBJECT

    const GSampleTable* table;

public:
    PlotTableDialog(const GSampleTable* table, EnumIodeGraphChart chartType = I_G_CHART_LINE, const bool logScale = false, 
        EnumIodeGraphAxisThicks xTicks = I_G_MAJOR_THICKS, EnumIodeGraphAxisThicks yTicks = I_G_MAJOR_THICKS, 
        QWidget* parent = nullptr);
    ~PlotTableDialog();

protected:
    QString defaultTitle()
    {
        return QString::fromStdString(table->get_title());
    }
};