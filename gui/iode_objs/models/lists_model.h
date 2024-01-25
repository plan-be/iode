#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class ListsModel : public IodeTemplateTableModel<KDBLists>
{
	Q_OBJECT

public:
	ListsModel(QObject* parent = nullptr, KDBLists* database = nullptr) 
		: IodeTemplateTableModel({"List"}, database, parent) {};

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;
};
