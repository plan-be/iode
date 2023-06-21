#pragma once
#include "plot.h"


class QIodePlotVariablesDialog : public QIodePlotDialog
{
    Q_OBJECT

    KDBVariables* kdb_vars;
    EnumIodeGraphAxisType axisType;

    QComboBox* comboXAxisType;

public:
    QIodePlotVariablesDialog(KDBVariables* kdb_vars = nullptr, EnumIodeGraphChart chartType = I_G_CHART_LINE, 
        EnumIodeGraphAxisType axisType = I_G_LEVEL, const bool logScale = false, EnumIodeGraphAxisThicks xTicks = I_G_MAJOR_THICKS, 
        EnumIodeGraphAxisThicks yTicks = I_G_MAJOR_THICKS, QWidget* parent = nullptr);
    ~QIodePlotVariablesDialog();

    void addSeries(const QString& variable, const QString& legend_name = "");

protected:
    QString defaultTitle()
    {
        QString s_axisType = QString::fromStdString(vGraphsXAxisTypes[axisType]);
        return chart_series.keys().join(" - ") + " (" + s_axisType + ")";
    }

public slots:
    void updateXAxisType(int index);

};