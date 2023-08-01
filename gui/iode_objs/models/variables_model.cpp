#pragma once
#include "variables_model.h"


QVariant VariablesModel::dataCell(const int row, const int col) const
{
	int mode = 0;
	IODE_REAL var;

	try
	{
		var = kdb->get_var(row, col, mode);
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
		double val = (value == NAN_REP || value == "") ? L_NAN : value.toDouble();
		kdb->set_var(row, column, val);
		return true;
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("WARNING"), tr(e.what()));
		return false;
	}
}

void VariablesModel::reset()
{
	QString period;
	columnNames = QVector<QString>();
	for (int t = 0; t < kdb->get_nb_periods(); t++) {
		period = QString::fromStdString(kdb->get_period(t).to_string());
		columnNames.append(period);
	}

	QIodeAbstractTableModel::reset();
}
