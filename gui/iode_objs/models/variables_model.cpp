#pragma once
#include "variables_model.h"


QVariant VariablesModel::dataCell(const int row, const int col) const
{
	int mode = 0;
	IODE_REAL var;

	if (col == 0)
	{
		return QString::fromStdString(kdb->get_name(row));
	}
	else
	{
		var = kdb->get_var(row, col - 1, mode);
		return L_ISAN(var) ? QString::number(var, 'g', 3) : NAN_REP;
	}
}

void VariablesModel::reset()
{
	QString period;
	columnNames = QVector<QString>({ "Name" });
	for (int t = 0; t < kdb->get_nb_periods(); t++) {
		period = QString::fromStdString(kdb->get_period(t).to_string());
		columnNames.append(period);
	}
	resetModel();
}
