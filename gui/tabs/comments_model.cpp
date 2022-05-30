#pragma once
#include "comments_model.h"


QVariant CommentsModel::dataCell(const int row, const int col) const
{
	if (col == 0)
	{
		return QVariant(QString::fromStdString(kdb.get_name(row)));
	}
	else
	{
		return QVariant(QString::fromStdString(kdb.get(row)));
	}
}

bool CommentsModel::setValue(const int row, const int column, const QVariant& value)
{
	try
	{
		kdb.update(row, value.toString().toStdString());
		return true;
	}
	catch (const std::runtime_error& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
		return false;
	}
}
