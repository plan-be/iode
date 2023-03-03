#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class TablesModel : public IODEAbstractTableModel<KDBTables>
{
	Q_OBJECT

public:
	TablesModel(QObject* parent = nullptr) : IODEAbstractTableModel({"Table"}, parent) {};

private:
	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;

public slots:
	void reset() { resetModel(); };
};
