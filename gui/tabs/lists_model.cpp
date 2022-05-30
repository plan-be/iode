#pragma once
#include "lists_model.h"


QVariant ListsModel::dataCell(const int row, const int col) const
{
	if (col == 0)
		return QVariant(QString::fromStdString(kdb.get_name(row)));
	else
		return QVariant(QString::fromStdString(kdb.get(row)));
}
