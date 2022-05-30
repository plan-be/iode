#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class VariablesModel : public IODEAbstractTableModel<KDBVariables>
{
	Q_OBJECT

public:
	VariablesModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const;

public slots:
	void reset();
};
