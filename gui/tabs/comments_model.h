#pragma once

#include <QObject>

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
            return QVariant(QString(kdb.getObjectName(row)));
        }
        else
        {
            return QVariant(QString(kdb.getObjectValue(row)));
        }
	}

    bool setValue(const int row, const int column, const QVariant& value) override
    { 
		try
		{
			QString s_value = value.toString();
			char* comment = new char[s_value.size() + 1];
			strcpy(comment, s_value.toUtf8().data());
			kdb.setObjectValue(row, comment);
			delete[] comment;
			return true;
		}
		catch (const std::runtime_error& e)
		{
			QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
			return false;
		}
    }

public slots:
	void reset() { resetModel(); };
};
