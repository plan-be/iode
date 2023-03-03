#pragma once

#include <QObject>

#include "abstract_table_model.h"
#include "abstract_table_model.cpp"


class CommentsModel : public IODEAbstractTableModel<KDBComments>
{
	Q_OBJECT

public:
	CommentsModel(QObject* parent = nullptr) : IODEAbstractTableModel({"Comment" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const override;

	bool setValue(const int row, const int column, const QVariant& value) override;

public slots:
	void reset() { resetModel(); };
};
