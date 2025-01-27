#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class TablesModel : public IodeTemplateTableModel<KDBTables>
{
	Q_OBJECT

public:
	TablesModel(QObject* parent = nullptr, KDBTables* database = nullptr) 
		: IodeTemplateTableModel({"Table"}, database, parent) {};

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;
};
