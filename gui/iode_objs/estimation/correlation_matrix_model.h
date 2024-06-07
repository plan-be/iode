#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>

#include "utils.h"
#include "cpp_api/compute/estimation.h"


/* NOTE FOR THE DEVELOPERS:
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
    CorrelationMatrix* corr_matrix;

public:
    CorrelationMatrixModel(CorrelationMatrix* corr_matrix, QObject* parent = nullptr) : 
        QAbstractTableModel(parent), corr_matrix(corr_matrix) {}

	// WARNING: Do not delete corr_matrix has it deleted by the instance of EditAndEstimateEquations
    ~CorrelationMatrixModel() {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const
	{
		return corr_matrix->nb_coeffs;
	}

	int columnCount(const QModelIndex& parent = QModelIndex()) const
	{
		return corr_matrix->nb_coeffs;
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
};
