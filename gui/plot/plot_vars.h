#pragma once
#include "plot.h"


class PlotVariablesDialog : public PlotDialog
{
    Q_OBJECT

    KDBVariables* kdb_vars;
    IodeVarMode varMode;

    QComboBox* comboVarMode;

public:
    PlotVariablesDialog(KDBVariables* kdb_vars = nullptr, TableGraphType chartType = TABLE_GRAPH_LINE, 
        IodeVarMode varMode = VAR_MODE_LEVEL, const bool logScale = false, TableGraphGrid xTicks = TABLE_GRAPH_MAJOR, 
        TableGraphGrid yTicks = TABLE_GRAPH_MAJOR, QWidget* parent = nullptr);
    ~PlotVariablesDialog();

    void addSeries(const QString& variable, const QString& legend_name = "");

protected:
    QString defaultTitle()
    {
        QString title = chart_series[0].name;

        for(int i = 1; i < chart_series.size(); i++)
            title += " - " + chart_series[i].name;
        
        title += " (" + QString::fromStdString(v_var_modes[varMode]) + ")";
        return title;
    }

public slots:
    void updateVarMode(int index);

};