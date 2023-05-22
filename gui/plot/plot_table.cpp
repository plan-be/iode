#include "plot_table.h"


QIodePlotTableDialog::QIodePlotTableDialog(const GSampleTable* table, EnumIodeGraphChart chartType, const bool logScale, 
    EnumIodeGraphAxisThicks xTicks, EnumIodeGraphAxisThicks yTicks, QWidget* parent, Qt::WindowFlags f)
    : QIodePlotDialog(chartType, logScale, xTicks, yTicks, parent, f), table(table)
{
    QString key;

    Sample* sample = table->get_sample();
    setPeriods(*sample);

    for(int row = 0; row < table->get_nb_lines(); row++)
    {
        key = "line_" + QString::number(row);

        legend_series[key] = QString::fromStdString(table->get_line_name(row));

        QVector<double> values;
        for(int col = 0; col < table->get_nb_columns(); col++)
            values.append(table->get_value(row, col, 8));
        chart_series[key] = values;
    }
}

QIodePlotTableDialog::~QIodePlotTableDialog()
{
}
