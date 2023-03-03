#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class IdentitiesModel : public IODEAbstractTableModel<KDBIdentities>
{
	Q_OBJECT

public:
	IdentitiesModel(QObject* parent = nullptr) : IODEAbstractTableModel({"Identity"}, parent) {};

private:
	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;

public slots:
	void reset() { resetModel(); };
};
