#include "correlation_matrix_model.h"


QVariant CorrelationMatrixModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    return QString::fromStdString(corr_matrix->get_name(section));
}

QVariant CorrelationMatrixModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount() || index.column() >= columnCount())
        return QVariant();

	if (role == Qt::TextAlignmentRole)
		return int(Qt::AlignRight);

    if (role == Qt::DisplayRole)
        return QString::number(corr_matrix->get_value(index.row(), index.column()), 'g', 6);
    else
        return QVariant();
}