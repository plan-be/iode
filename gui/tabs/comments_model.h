#pragma once

#include <QObject>
#include <QMessageBox>

#include "abstract_table_model.h"


class CommentsModel : public IODEAbstractTableModel<Comments>
{
	Q_OBJECT

public:
	CommentsModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "Comment" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const
	{
        if (col == 0)
        {
            return QVariant(QString(iodeItems.getObjectName(row)));
        }
        else
        {
            return QVariant(QString(iodeItems.getObjectValue(row)));
        }
	}

public slots:
	void reset() { resetModel(); };
};
