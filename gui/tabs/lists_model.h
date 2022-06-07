#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class ListsModel : public IODEAbstractTableModel<KDBLists>
{
	Q_OBJECT

public:
	ListsModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "List" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const override;

public slots:
	void reset() { resetModel(); };
};
