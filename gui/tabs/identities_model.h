#pragma once

#include <QObject>

#include "abstract_table_model.h"


class IdentitiesModel : public IODEAbstractTableModel<Identities>
{
	Q_OBJECT

public:
	IdentitiesModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "Identity" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const
	{
		if (col == 0)
			return QVariant(QString(iodeItems.getObjectName(row)));
		else
			return QVariant(QString(iodeItems.getLec(row)));
	}

public slots:
	void reset() { resetModel(); };
};
