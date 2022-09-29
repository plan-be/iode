#pragma once
#include "abstract_table_model.h"


template <class K>
Qt::ItemFlags IODEAbstractTableModel<K>::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

template <class K>
QVariant IODEAbstractTableModel<K>::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return columnNames[section];

	return QVariant("  ");
}

template <class K>
QVariant IODEAbstractTableModel<K>::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::TextAlignmentRole)
		return int(Qt::AlignLeft);

	if (role == Qt::DisplayRole || role == Qt::EditRole)
		return dataCell(index.row(), index.column());

	return QVariant();
}

template <class K>
bool IODEAbstractTableModel<K>::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (index.isValid() && role == Qt::EditRole)
	{
		bool success = setDataCell(index.row(), index.column(), value);
		if (success)
		{
			emit dataChanged(index, index, { role });
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

template <class K>
bool IODEAbstractTableModel<K>::rename(const QString& name, const QString& new_name)
{
	try
	{
		kdb->rename(name.toStdString(), new_name.toStdString());
		return true;
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
		return false;
	}
}

template <class K>
bool IODEAbstractTableModel<K>::setDataCell(const int row, const int column, const QVariant& value)
{
	if (column == 0)
	{
		QString name = QString::fromStdString(kdb->get_name(row));
		return rename(name, value.toString());
	}
	else
	{
		return setValue(row, column, value);
	}
}

template <class K>
void IODEAbstractTableModel<K>::filter(const QString& pattern)
{
	try
	{
		if (kdb != nullptr) delete kdb;
		if (!pattern.isEmpty())
		{
			kdb = new K(KDB_SHALLOW_COPY, pattern.toStdString());
		}
		else
		{
			kdb = new K(KDB_GLOBAL);
		}
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(static_cast<QWidget*>(parent()), tr("Error"), tr(e.what()));
	}
}

template <class K>
bool IODEAbstractTableModel<K>::removeRows(int position, int rows, const QModelIndex& index)
{
	std::string name;
	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	try
	{
		for (int row = position; row < position + rows; row++)
		{
			name = dataCell(row, 0).toString().toStdString();
			kdb->remove(row);
		}
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}

	endRemoveRows();
	return true;
}
