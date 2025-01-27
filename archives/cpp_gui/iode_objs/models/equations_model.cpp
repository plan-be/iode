#pragma once
#include "equations_model.h"


QVariant EquationsModel::dataCell(const int row, const int col) const
{
	try
	{
		return QVariant(QString::fromStdString(displayed_database->get_lec(row)));
	}
	catch(const std::exception& e)
	{
		return QVariant(" ");
	}
}

bool EquationsModel::setValue(const int row, const int column, const QVariant& value)
{
	try
	{
		QString lec = value.toString();
		displayed_database->update(row, lec.toStdString());
		return true;
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString::fromStdString(e.what()));
	}
	return false;
}
