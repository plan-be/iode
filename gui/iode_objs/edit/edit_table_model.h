#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QMessageBox>
#include <QAbstractTableModel>

#include "utils.h"


class QIodeEditTableModel : public QAbstractTableModel
{
	Q_OBJECT

	QString tableName;
    Table* table;

public:
    QIodeEditTableModel(const QString& tableName, QWidget* parent = Q_NULLPTR);

	~QIodeEditTableModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const;

	int columnCount(const QModelIndex& parent = QModelIndex()) const;

	Qt::ItemFlags flags(const QModelIndex& index) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	bool setData(const QModelIndex& index, const QVariant& value, int role);

	EnumLineType getLineType(const int row) const;

	void save();

protected:
	void resetModel()
	{
		beginResetModel();
		endResetModel();
	}

signals:
	void displayData(int row, EnumLineType line_type, int nb_columns) const;
};
