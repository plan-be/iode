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
#include "custom_widgets/qfilechooser.h"


/* NOTE FOR THE DEVELOPPERS:
 * Because of how Qt is implemented, it is NOT possible for a template class
 * to define/override signals and slots (using Q_OBJECT)
 * ( see https://doc.qt.io/qt-5/moc.html#limitations )
 * 
 * Read-only model: https://doc.qt.io/qt-6/model-view-programming.html#read-only-access
 * 
 */

class GSampleTableModel : public QAbstractTableModel
{
    QString refTable;
    QString variables;
    int nbDecimals;

    GSampleTable *table;

public:

    GSampleTableModel(const QString& refTable, const QString& gsample, const int nbDecimals, const QString& variables);
    ~GSampleTableModel();

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
};
