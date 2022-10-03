#include "correlation_matrix_model.h"


QVariant CorrelationMatrixModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    return coefs_names[section];
}

QVariant CorrelationMatrixModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > coefs_names.size() || index.column() > coefs_names.size())
        return QVariant();

    if (role == Qt::DisplayRole)
        return QString::number(MATE(corr_matrix, index.row(), index.column()), 'g', 6);
    else
        return QVariant();
}