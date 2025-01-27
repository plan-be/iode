#pragma once
#include "variables_model.h"


QVariant VariablesModel::dataCell(const int row, const int col) const
{
	double var;

	try
	{
		var = displayed_database->get_var(row, col, mode);
		return valueToString(var);
	}
	catch(const std::exception& e)
	{
		return QVariant(" ");
	}
}

bool VariablesModel::setValue(const int row, const int column, const QVariant& value)
{
	try
	{
		double val = (value == NAN_REP || value == "") ? IODE_NAN : value.toDouble();
		displayed_database->set_var(row, column, val);
		return true;
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
		return false;
	}
}

void VariablesModel::reset()
{
	columnNames = QVector<QString>();
	for (const std::string& period: Variables.get_list_periods()) 
		columnNames.append(QString::fromStdString(period));

	IodeAbstractTableModel::reset();
}
