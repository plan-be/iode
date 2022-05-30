#pragma once
#include "tables_model.h"


QVariant TablesModel::dataCell(const int row, const int col) const
{
	if (col == 0)
		return QVariant(QString::fromStdString(kdb.get_name(row)));
	else
		return QVariant(QString::fromStdString(kdb.get_title(row)));
}
