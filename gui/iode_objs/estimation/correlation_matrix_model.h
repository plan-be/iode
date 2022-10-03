#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>

#include "utils.h"


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


class CorrelationMatrixModel : public QAbstractTableModel
{
    MAT* corr_matrix;
    QVector<QString> coefs_names;

public:
    CorrelationMatrixModel(const QVector<QString>& coefs_names, MAT* corr_matrix, QObject* parent = nullptr) : 
        QAbstractTableModel(parent), coefs_names(coefs_names), corr_matrix(corr_matrix) {}

    ~CorrelationMatrixModel() {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const
	{
		return coefs_names.size();
	}

	int columnCount(const QModelIndex& parent = QModelIndex()) const
	{
		return coefs_names.size();
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
};
