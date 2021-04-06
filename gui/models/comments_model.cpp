#include "comments_model.h"


CommentsModel::CommentsModel(QObject* parent) : QAbstractTableModel(parent)
{
}

QVariant CommentsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole)
        return int(Qt::AlignLeft);

    if (role == Qt::DisplayRole)
    {
        int pos = index.row();
        if (index.column() == 0)
            return QString::fromLatin1(get_comment_name(pos));
        else
            return QString::fromLatin1(get_comment_value(pos));
    }
    else
        return QVariant();
}

QVariant CommentsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) 
    {
        switch (section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("Comment");
        default:
            break;
        }
    }

    return QVariant("  ");
}

void CommentsModel::reset()
{
    beginResetModel();
    endResetModel();
}
