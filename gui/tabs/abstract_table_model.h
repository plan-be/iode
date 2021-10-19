#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>

#include "../utils.h"
#include "../bridge.h"


/* NOTE FOR THE DEVELOPPERS:
 * Because of how Qt is implemented, it is NOT possible for a template class
 * to define/override signals and slots (using Q_OBJECT)
 * ( see https://doc.qt.io/qt-5/moc.html#limitations )
 * 
 * Read-only model: https://doc.qt.io/qt-6/model-view-programming.html#read-only-access
 * Editable model:  https://doc.qt.io/qt-6/model-view-programming.html#editable-items
 * Resizable model: https://doc.qt.io/qt-6/model-view-programming.html#resizable-models
 * 
 */


const static char NAN_REP[3] = "--";


template <class I> class IODEAbstractTableModel : public QAbstractTableModel
{
protected:
	I iodeItems;
	QVector<QString> columnNames;

public:
	IODEAbstractTableModel(QVector<QString> columnNames, QObject* parent = nullptr) : QAbstractTableModel(parent),
		iodeItems(I()), columnNames(columnNames) {};

	int rowCount(const QModelIndex& parent = QModelIndex()) const
	{
		return iodeItems.count();
	}

	int columnCount(const QModelIndex& parent = QModelIndex()) const
	{
		return columnNames.size();
	}

	Qt::ItemFlags flags(const QModelIndex& index) const
	{
		if (!index.isValid())
			return Qt::ItemIsEnabled;

		return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const
	{
		if (role != Qt::DisplayRole)
			return QVariant();

		if (orientation == Qt::Horizontal)
			return columnNames[section];

		return QVariant("  ");
	}

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
	{
		if (!index.isValid())
			return QVariant();

		if (role == Qt::TextAlignmentRole)
			return int(Qt::AlignLeft);

		if (role == Qt::DisplayRole || role == Qt::EditRole)
			return dataCell(index.row(), index.column());
		else
			return QVariant();
	}

	bool setData(const QModelIndex& index, const QVariant& value, int role)
	{
		if (index.isValid() && role == Qt::EditRole) 
		{

			bool success = setDataCcell(index.row(), index.column(), value);
			if (success)
			{
				emit dataChanged(index, index, { role });
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

protected:
	void resetModel()
	{
		beginResetModel();
		endResetModel();
	}

	virtual QVariant dataCell(const int row, const int col) const = 0;
	bool setDataCcell(const int row, const int column, const QVariant& value) { return false; }
};
