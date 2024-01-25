#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class IdentitiesModel : public IodeTemplateTableModel<KDBIdentities>
{
	Q_OBJECT

public:
	IdentitiesModel(QObject* parent = nullptr, KDBIdentities* database = nullptr) 
		: IodeTemplateTableModel({"Identity"}, database, parent) {};

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;

	void executeIdentity(const int row);
};
