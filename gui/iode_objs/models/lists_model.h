#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class ListsModel : public QIodeTemplateTableModel<KDBLists>
{
	Q_OBJECT

public:
	ListsModel(QObject* parent = nullptr) : QIodeTemplateTableModel({"List"}, parent) {};

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;

public slots:
	void reset() { resetModel(); };
};
