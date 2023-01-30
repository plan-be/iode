#include "gsample_table_model.h"


GSampleTableModel::GSampleTableModel(const QString& refTable, const QString& gsample, const int nbDecimals, const QString& variables) 
    : refTable(refTable), nbDecimals(nbDecimals)
{
    if(!variables.isEmpty())
    {
        KDBTables kdb_tables;
        kdb_tables.add(refTable.toStdString(), 2, "", variables.toStdString(), false, false, false);
    }

    table = new GSampleTable(refTable.toStdString(), gsample.toStdString());
}

GSampleTableModel::~GSampleTableModel()
{
    delete table;
    if(!variables.isEmpty())
    {
        KDBTables kdb_tables;
        kdb_tables.remove(refTable.toStdString());
    }
}

Qt::ItemFlags GSampleTableModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant GSampleTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString::fromStdString(table->get_column_name(section));
    else
        return QString::fromStdString(table->get_line_name(section));
}

QVariant GSampleTableModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::TextAlignmentRole)
		return int(Qt::AlignLeft);

	if (role == Qt::DisplayRole || role == Qt::EditRole)
		return QVariant(table->get_value(index.row(), index.column(), nbDecimals));

	return QVariant();
}