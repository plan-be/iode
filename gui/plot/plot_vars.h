#pragma once
#include "plot.h"


class QIodePlotVariablesDialog : public QIodePlotDialog
{
    Q_OBJECT

    KDBVariables* kdb_vars;
    EnumIodeVarMode varMode;

    QComboBox* comboVarMode;

public:
    QIodePlotVariablesDialog(KDBVariables* kdb_vars = nullptr, EnumIodeGraphChart chartType = I_G_CHART_LINE, 
        EnumIodeVarMode varMode = I_VAR_MODE_LEVEL, const bool logScale = false, EnumIodeGraphAxisThicks xTicks = I_G_MAJOR_THICKS, 
        EnumIodeGraphAxisThicks yTicks = I_G_MAJOR_THICKS, QWidget* parent = nullptr);
    ~QIodePlotVariablesDialog();

    void addSeries(const QString& variable, const QString& legend_name = "");

protected:
    QString defaultTitle()
    {
        QString s_varMode = QString::fromStdString(v_var_modes[varMode]);
        return chart_series.keys().join(" - ") + " (" + s_varMode + ")";
    }

public slots:
    void updateVarMode(int index);

};