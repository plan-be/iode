#pragma once
#include "lists_model.h"


QVariant ListsModel::dataCell(const int row, const int col) const
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

bool ListsModel::setValue(const int row, const int column, const QVariant& value)
{
	try
	{
		kdb->update(row, value.toString().toStdString());
		return true;
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("WARNING"), tr(e.what()));
		return false;
	}
}
