#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QAbstractTableModel>
#include <QDebug>

#include "utils.h"
#include "util/widgets/file_chooser.h"
#include "numerical_table_model.h"


class ComputedTableModel : public QAbstractTableModel, public NumericalTableModel
{
    QString refTable;
    QString variables;

    ComputedTable* table;

public:

    ComputedTableModel(const QString& refTable, const QString& gsample, const int nbDecimals, 
		const QString& variables, QObject *parent);
    ~ComputedTableModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const override
	{
		return table->get_nb_lines();
	}

	int columnCount(const QModelIndex& parent = QModelIndex()) const override
	{
		return table->get_nb_columns();
	}

	Qt::ItemFlags flags(const QModelIndex& index) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	void updateDisplayValues() override
	{
		reset();
	}

public slots:
	void reset()
	{
		beginResetModel();
		endResetModel();
	}
};
