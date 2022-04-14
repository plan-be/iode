#pragma once

#include <QObject>

#include "abstract_table_model.h"


class IdentitiesModel : public IODEAbstractTableModel<KDBIdentities>
{
	Q_OBJECT

public:
	IdentitiesModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "Identity" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const
	{
		if (col == 0)
			return QVariant(QString::fromStdString(kdb.get_name(row)));
		else
			return QVariant(QString::fromStdString(kdb.get_lec(row)));
	}

public slots:
	void reset() { resetModel(); };
};
