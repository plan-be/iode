#pragma once
#include "plot.h"


class PlotVariablesDialog : public PlotDialog
{
    Q_OBJECT

    KDBVariables* kdb_vars;
    EnumIodeVarMode varMode;

    QComboBox* comboVarMode;

public:
    PlotVariablesDialog(KDBVariables* kdb_vars = nullptr, EnumIodeGraphChart chartType = I_G_CHART_LINE, 
        EnumIodeVarMode varMode = I_VAR_MODE_LEVEL, const bool logScale = false, EnumIodeGraphAxisThicks xTicks = I_G_MAJOR_THICKS, 
        EnumIodeGraphAxisThicks yTicks = I_G_MAJOR_THICKS, QWidget* parent = nullptr);
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