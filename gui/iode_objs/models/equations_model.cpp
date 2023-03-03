#pragma once
#include "equations_model.h"


QVariant EquationsModel::dataCell(const int row, const int col) const
{
	try
	{
		return QVariant(QString::fromStdString(kdb->get_lec(row)));
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
		kdb->update(row, lec.toStdString());
		return true;
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(nullptr, "Warning", QString::fromStdString(e.what()));
	}
	return false;
}
