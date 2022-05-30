#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QMessageBox>
#include <QAbstractTableModel>

#include "../utils.h"


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


template <class K> class IODEAbstractTableModel : public QAbstractTableModel
{
protected:
	K kdb;
	QVector<QString> columnNames;

public:
	IODEAbstractTableModel(QVector<QString> columnNames, QObject* parent = nullptr) : QAbstractTableModel(parent),
		kdb(K()), columnNames(columnNames) {};

	int rowCount(const QModelIndex& parent = QModelIndex()) const
	{
		return kdb.count();
	}

	int columnCount(const QModelIndex& parent = QModelIndex()) const
	{
		return columnNames.size();
	}

	Qt::ItemFlags flags(const QModelIndex& index) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	bool setData(const QModelIndex& index, const QVariant& value, int role);

protected:
	void resetModel()
	{
		beginResetModel();
		endResetModel();
	}

	virtual QVariant dataCell(const int row, const int col) const = 0;

	bool setName(const int row, const QString& new_name);

	virtual bool setValue(const int row, const int column, const QVariant& value) { return false; }

	bool setDataCell(const int row, const int column, const QVariant& value);

	bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;
};
