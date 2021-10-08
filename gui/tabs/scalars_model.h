#pragma once

#include <QObject>

#include "abstract_table_model.h"


class ScalarsModel : public IODEAbstractTableModel<Scalars>
{
	Q_OBJECT

public:
	ScalarsModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "Value", "Relax", "Std", "T-Stat" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const
	{
		QVariant value;
		Scalar* scalar = iodeItems.getObjectValue(row);

		switch (col)
		{
		case 0:
			value = QString::fromLatin1(iodeItems.getObjectName(row));
			break;
		case 1:
			value = L_ISAN(scalar->val) ? QString::number(scalar->val, 'g', 3) : NAN_REP;
			break;
		case 2:
			value = L_ISAN(scalar->relax) ? QString::number(scalar->relax, 'g', 3) : NAN_REP;
			break;
		case 3:
			value = L_ISAN(scalar->std) ? QString::number(scalar->std, 'g', 3) : NAN_REP;
			break;
		case 4:
			if (L_ISAN(scalar->val) && L_ISAN(scalar->std) && !L_IS0(scalar->std))
				value = QString::number(scalar->val / scalar->std, 'g', 3);
			else
				value = NAN_REP;
			break;
		default:
			break;
		}
		return value;
	}

public slots:
	void reset() { resetModel(); };
};
