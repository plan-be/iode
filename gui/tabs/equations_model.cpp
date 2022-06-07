#pragma once
#include "equations_model.h"


QVariant EquationsModel::dataCell(const int row, const int col) const
{
	if (col == 0)
		return QString::fromStdString(kdb->get_name(row));
	else
		return QVariant(QString::fromStdString(kdb->get_lec(row)));
}
