#pragma once

#include <QObject>
#include <QWidget>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QMessageBox>
#include <QAbstractTableModel>

#include <string>
#include <stdexcept>
#include <exception>

#include "../utils.h"
#include "../bridge.h"


/* NOTE FOR THE DEVELOPPERS:
 * Because of how Qt is implemented, it is NOT possible for a template class
 * to define/override signals and slots (using Q_OBJECT)
 * ( see https://doc.qt.io/qt-5/moc.html#limitations )
 */


constexpr char NAN_REP[3] = "--";


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

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
	{
		if (!index.isValid())
			return QVariant();

		if (role == Qt::TextAlignmentRole)
			return int(Qt::AlignLeft);

		if (role == Qt::DisplayRole)
			return dataCell(index.row(), index.column());
		else
			return QVariant();
	}
	
	QVariant headerData(int section, Qt::Orientation orientation, int role) const
	{
		if (role != Qt::DisplayRole)
			return QVariant();

		if (orientation == Qt::Horizontal)
			return columnNames[section];

		return QVariant("  ");
	}

protected:
	void resetModel()
	{
		beginResetModel();
		endResetModel();
	}

	virtual QVariant dataCell(const int row, const int col) const = 0;
};
