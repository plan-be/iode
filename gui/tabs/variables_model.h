#pragma once

#include <QObject>

#include "abstract_table_model.h"


class VariablesModel : public IODEAbstractTableModel<Variables>
{
	Q_OBJECT

public:
	VariablesModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const
	{
		int mode = 0;
		IODE_REAL var;

		if (col == 0) 
		{
			return QVariant(QString(kdb.getObjectName(row)));
		}
		else 
		{
			var = kdb.getValue(row, col - 1, mode);
			return L_ISAN(var) ? QString::number(var, 'g', 3) : NAN_REP;
		}
	}

public slots:
	void reset()
	{
		columnNames = QVector<QString>({ "Name" });
		for (int t = 0; t < kdb.getNbPeriods(); t++) {
			char period[10];
			kdb.getPeriod(period, t);
			columnNames.append(period);
		}
		resetModel();
	};
};
