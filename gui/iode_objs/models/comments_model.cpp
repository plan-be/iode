#pragma once
#include "comments_model.h"


QVariant CommentsModel::dataCell(const int row, const int col) const
{
	try
	{
		return QVariant(QString::fromStdString(kdb->get(row)));
	}
	catch(const std::exception& e)
	{
		return QVariant(" ");
	}
}

bool CommentsModel::setValue(const int row, const int column, const QVariant& value)
{
	try
	{
		kdb->update(row, value.toString().toStdString());
		return true;
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
		return false;
	}
}
