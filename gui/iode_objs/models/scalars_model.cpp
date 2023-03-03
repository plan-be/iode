#pragma once
#include "scalars_model.h"


QVariant ScalarsModel::dataCell(const int row, const int col) const
{
	QVariant value;

	try
	{
		Scalar scalar = kdb->get(row);

		switch (col)
		{
		case 0:
			value = L_ISAN(scalar.value()) ? QString::number(scalar.value(), 'g', 3) : NAN_REP;
			break;
		case 1:
			value = L_ISAN(scalar.relax()) ? QString::number(scalar.relax(), 'g', 3) : NAN_REP;
			break;
		case 2:
			value = L_ISAN(scalar.std()) ? QString::number(scalar.std(), 'g', 3) : NAN_REP;
			break;
		case 3:
			if (L_ISAN(scalar.value()) && L_ISAN(scalar.std()) && !L_IS0(scalar.std()))
				value = QString::number(scalar.value() / scalar.std(), 'g', 3);
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
			kdb->update(row, val, scalar.relax());
			break;
		case 1:
			kdb->update(row, scalar.value(), val);
			break;
		default:
			break;
		}

		return true;
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
		return false;
	}
}
