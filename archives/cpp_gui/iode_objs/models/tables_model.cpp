#pragma once
#include "tables_model.h"


QVariant TablesModel::dataCell(const int row, const int col) const
{
	try
	{
		return QVariant(QString::fromStdString(displayed_database->get_title(row)));
	}
	catch(const std::exception& e)
	{
		return QVariant(" ");
	}
}

bool TablesModel::setValue(const int row, const int column, const QVariant& value)
{
	try
	{
		QString title = value.toString();
		Table* table = displayed_database->get(row);
		table->set_title(0, title.toStdString());
		displayed_database->update(row, table);
		delete table;
		return true;
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString::fromStdString(e.what()));
	}
	return false;
}
