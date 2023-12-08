#include "gsample_table_model.h"


GSampleTableModel::GSampleTableModel(const QString& refTable, const QString& gsample, const int nbDecimals, 
    const QString& variables, QObject *parent): 
    GSampleNumericalTableModel(nbDecimals, parent), refTable(refTable), variables(variables)
{
    if(!variables.isEmpty())
        Tables.add(refTable.toStdString(), 2, "", variables.toStdString(), false, false, false);

    table = new GSampleTable(refTable.toStdString(), gsample.toStdString());
}

GSampleTableModel::~GSampleTableModel()
{
    delete table;
    if(!variables.isEmpty())
        Tables.remove(refTable.toStdString());
}

Qt::ItemFlags GSampleTableModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

    Qt::ItemFlags flag = (table->is_editable(index.row(), index.column())) ? Qt::ItemIsEditable : Qt::NoItemFlags;

	return QAbstractItemModel::flags(index) | flag;
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
		return valueToString(table->get_value(index.row(), index.column(), get_precision()));

	return QVariant();
}

bool GSampleTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
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