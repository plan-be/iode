#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class CommentsModel : public IodeTemplateTableModel<KDBComments>
{
	Q_OBJECT

public:
	CommentsModel(QObject* parent = nullptr, KDBComments* kdb_external = nullptr) 
		: IodeTemplateTableModel({"Comment" }, parent, kdb_external) {};

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;
};
