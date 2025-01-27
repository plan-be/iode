#include "computed_table_model.h"


ComputedTableModel::ComputedTableModel(const QString& refTable, const QString& gsample, const int nbDecimals, 
    const QString& variables, QObject *parent): QAbstractTableModel(parent), NumericalTableModel(nbDecimals), 
    refTable(refTable), variables(variables)
{
    if(!variables.isEmpty())
        Tables.add(refTable.toStdString(), 2, "", variables.toStdString(), false, false, false);

	Table* table_obj = Tables.get(refTable.toStdString());
    table = new ComputedTable(table_obj, gsample.toStdString());
	delete table_obj;
}

ComputedTableModel::~ComputedTableModel()
{
	if(table)
    	delete table;
    
	if(!variables.isEmpty())
        Tables.remove(refTable.toStdString());
}

Qt::ItemFlags ComputedTableModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

    Qt::ItemFlags flag = (table->is_editable(index.row(), index.column())) ? Qt::ItemIsEditable : Qt::NoItemFlags;

	return QAbstractItemModel::flags(index) | flag;
}

QVariant ComputedTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString::fromStdString(table->get_column_name(section));
    else
        return QString::fromStdString(table->get_line_name(section));
}

QVariant ComputedTableModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::TextAlignmentRole)
		return int(Qt::AlignRight);

	if (role == Qt::DisplayRole || role == Qt::EditRole)
		return valueToString(table->get_value(index.row(), index.column(), get_precision()));

	return QVariant();
}

bool ComputedTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (index.isValid() && role == Qt::EditRole)
	{
		if(data(index, Qt::DisplayRole) == value) 
			return false;
		
        if(value.toString() == NAN_REP)
            return false;

        try
        {
            table->set_value(index.row(), index.column(), value.toDouble(), false);
			emit dataChanged(index, index, { role });
			return true;
        }
        catch(const std::exception& e)
        {
            QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
		    return false;
        }
	}
	else
		return false;
}