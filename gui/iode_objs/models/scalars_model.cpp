#pragma once
#include "scalars_model.h"


QVariant ScalarsModel::dataCell(const int row, const int col) const
{
	QString value;

	try
	{
		Scalar scalar = kdb->get(row);

		switch (col)
		{
		case 0:
			value = valueToString(scalar.val);
			break;
		case 1:
			value = valueToString(scalar.relax);
			break;
		case 2:
			value = valueToString(scalar.std);
			break;
		case 3:
			if (L_ISAN(scalar.val) && L_ISAN(scalar.std) && !L_IS0(scalar.std))
				value = valueToString(scalar.val / scalar.std);
			else
				value = NAN_REP;
			break;
		default:
			break;
		}
	}
	catch(const std::exception& e)
	{
		return QVariant(" ");
	}

	return value;
}

bool ScalarsModel::setValue(const int row, const int column, const QVariant& value)
{
	try
	{
		Scalar scalar = kdb->get(row);
		double val = (value == "--") ? L_NAN : value.toDouble();

		switch (column)
		{
		case 0:
			kdb->update(row, val, scalar.relax);
			break;
		case 1:
			kdb->update(row, scalar.val, val);
			break;
		default:
			break;
		}

		return true;
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("WARNING"), tr(e.what()));
		return false;
	}
}
