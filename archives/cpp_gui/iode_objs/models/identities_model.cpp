#pragma once
#include "identities_model.h"


QVariant IdentitiesModel::dataCell(const int row, const int col) const
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

bool IdentitiesModel::setValue(const int row, const int column, const QVariant& value)
{
	try
	{
		displayed_database->update(row, value.toString().toStdString());
		return true;
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
		return false;
	}
}

void IdentitiesModel::executeIdentity(const int row)
{
	QString name = headerData(row, Qt::Vertical, Qt::DisplayRole).toString();

	try
	{
		Sample* sample = Variables.get_sample();
		displayed_database->execute_identities(sample->start_period(), sample->end_period(), name.toStdString());
		QMessageBox::information(nullptr, "INFO", "Identity " + name + " successfully executed");
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", 
			"Could not execute identity " + name + ":\n\n" + QString::fromStdString(e.what()));
	}
}
