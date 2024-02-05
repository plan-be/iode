#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QMessageBox>
#include <QAbstractTableModel>

#include "utils.h"


/* NOTE FOR THE DEVELOPERS:
 * 
 * Read-only model: https://doc.qt.io/qt-6/model-view-programming.html#read-only-access
 * Editable model:  https://doc.qt.io/qt-6/model-view-programming.html#editable-items
 * Resizable model: https://doc.qt.io/qt-6/model-view-programming.html#resizable-models
 * 
 */

class EditTableModel : public QAbstractTableModel
{
	Q_OBJECT

    Table* table;
	size_t hashBefore;
	size_t hashAfter;	

signals:
	void databaseModified();

public:
    EditTableModel(const QString& tableName, QWidget* parent = Q_NULLPTR);

	~EditTableModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const;

	int columnCount(const QModelIndex& parent = QModelIndex()) const;

	Qt::ItemFlags flags(const QModelIndex& index) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	bool setData(const QModelIndex& index, const QVariant& value, int role);

	EnumLineType get_line_type(const int row) const;

	void save(const QString& name);

	void computeHash(const bool before=false)
	{
		boost::hash<TBL> table_hasher;
		if(before)
    		hashBefore = table_hasher(*table);
		else
		{
			hashAfter = table_hasher(*table);
			if(hashAfter != hashBefore) 
				emit databaseModified();
		}
	}

	int appendLine(EnumLineType lineType);

	int insert_line(EnumLineType lineType, const int position, const bool after);

protected:
	bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;

	void resetModel()
	{
		beginResetModel();
		endResetModel();
	}

signals:
	void displayData(int row, EnumLineType line_type, int nb_columns) const;
};
