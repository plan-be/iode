#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class VariablesModel : public QIodeTemplateTableModel<KDBVariables>
{
	Q_OBJECT

public:
	VariablesModel(QObject* parent = nullptr) : QIodeTemplateTableModel({}, parent) {};

	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;

public slots:
	void reset();
};
