#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class TablesModel : public IODEAbstractTableModel<KDBTables>
{
	Q_OBJECT

public:
	TablesModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "Table" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const override;

public slots:
	void reset() { resetModel(); };
};
