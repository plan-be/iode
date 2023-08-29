#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class ListsModel : public IodeTemplateTableModel<KDBLists>
{
	Q_OBJECT

public:
	ListsModel(QObject* parent = nullptr, KDBLists* kdb_external = nullptr) 
		: IodeTemplateTableModel({"List"}, parent, kdb_external) {};

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;
};
