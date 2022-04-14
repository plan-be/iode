#pragma once

#include <QObject>

#include "abstract_table_model.h"


class CommentsModel : public IODEAbstractTableModel<KDBComments>
{
	Q_OBJECT

public:
	CommentsModel(QObject* parent = nullptr) : IODEAbstractTableModel({ "Name", "Comment" }, parent) {};

private:
	QVariant dataCell(const int row, const int col) const
	{
        if (col == 0)
        {
            return QVariant(QString::fromStdString(kdb.get_name(row)));
        }
        else
        {
            return QVariant(QString::fromStdString(kdb.get(row)));
        }
	}

    bool setValue(const int row, const int column, const QVariant& value) override
    { 
		try
		{
			kdb.update(row, value.toString().toStdString());
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
