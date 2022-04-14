#pragma once
#include "edit_table_model.h"

QIodeEditTableModel::QIodeEditTableModel(const QString& tableName, QWidget* parent) : QAbstractTableModel(parent), tableName(tableName)
{
	try
	{
		table = new Table(tableName.toStdString());
	}
	catch (const std::runtime_error& e)
	{
		QMessageBox::critical(static_cast<QWidget*>(parent), tr("ERROR"), tr(e.what()));
	}
}

QIodeEditTableModel::~QIodeEditTableModel()
{
	delete table;
}

int QIodeEditTableModel::rowCount(const QModelIndex& parent) const
{
	// + 1 for the divider line
	return table->nbLines() + 1;
}

int QIodeEditTableModel::columnCount(const QModelIndex& parent) const
{
	return table->nbColumns();
}

Qt::ItemFlags QIodeEditTableModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QVariant QIodeEditTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	QString h_data;

	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Vertical)
	{
		if (section == 0)
		{
			h_data = "DIVIS";
		}
		else
		{
			EnumLineType line_type = table->getLineType(section - 1);
			switch (line_type)
			{
			case IT_TITLE:
				h_data = "TITLE";
				break;
			case IT_CELL:
				h_data = "CELLS";
				break;
			case IT_MODE:
				h_data = "MODE";
				break;
			case IT_FILES:
				h_data = "FILES";
				break;
			case IT_DATE:
				h_data = "DATE";
				break;
			default:
				h_data = " ";
				break;
			}
		}
	}
	else
	{
		if (section == 0) h_data = "Line Titles";
		if (section == 1) h_data = "Formulas";
	}
	return QVariant(h_data);
}

QVariant QIodeEditTableModel::data(const QModelIndex& index, int role) const
{
	QString s_data;

	if (!index.isValid())
		return QVariant();

	bool is_divider = index.row() == 0;
	int i_line = index.row() - 1;
	int j_column = index.column();
	EnumLineType line_type = is_divider ? IT_CELL : table->getLineType(i_line);

	if (role == Qt::TextAlignmentRole)
	{
		EnumCellAttribute cell_align;
		switch (line_type)
		{
		case IT_TITLE:
			return int(Qt::AlignCenter);
		case IT_CELL:
			cell_align = is_divider ? table->getDividerCellAlign(j_column) : table->getCellAlign(i_line, j_column);
			switch (cell_align)
			{
			case IT_LEFT:
				return int(Qt::AlignLeft);
			case IT_CENTER:
				return int(Qt::AlignCenter);
			case IT_RIGHT:
				return int(Qt::AlignRight);
				break;
			default:
				return int(Qt::AlignLeft);
			}
		default:
			return int(Qt::AlignLeft);
		}
	}

	if (role == Qt::FontRole)
	{
		EnumCellAttribute cell_font;
		switch (line_type)
		{
		case IT_TITLE:
			return int(QFont::Bold);
		case IT_CELL:
			cell_font = is_divider ? table->getDividerCellFont(j_column) : table->getCellFont(i_line, j_column);
			switch (cell_font)
			{
			case IT_BOLD:
				return int(QFont::Bold);
			case IT_ITALIC:
				return int(QFont::StyleItalic);
			default:
				return int(QFont::StyleNormal);
			}
		default:
			return int(QFont::StyleNormal);
		}
	}

	if (role == Qt::BackgroundRole)
	{
		QColor color;
		switch (line_type)
		{
		case IT_LINE:
			color = QColor(Qt::black);
			break;
		case IT_CELL:
			color = QColor(Qt::white);
			break;
		default:
			color = QColor(Qt::lightGray);
			color.setAlphaF(0.4);
			break;
		}
		return QVariant(color);
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		QString s_data;
		if (is_divider)
		{
			s_data = QString::fromStdString(table->getDividerCellContent(j_column, false));
		}
		else
		{
			switch (line_type)
			{
			case IT_TITLE:
				s_data = QString::fromStdString(table->getTitle(i_line));
				break;
			case IT_CELL:
				s_data = QString::fromStdString(table->getCellContent(i_line, j_column, false));
				break;
			default:
				break;
			}
		}
		emit displayData(index.row(), line_type, columnCount());
		return QVariant(s_data);
	}

	return QVariant();
}

bool QIodeEditTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (index.isValid() && role == Qt::EditRole)
	{
		std::string std_value = value.toString().toStdString();
		try
		{
			bool is_divider = index.row() == 0;
			int i_line = index.row() - 1;
			int j_column = index.column();

			if (is_divider)
			{
				if (table->getDividerCellType(j_column) == IT_STRING)
					table->setCellDividerText(j_column, std_value);
				else
					table->setCellDividerLec(j_column, std_value);
			}
			else
			{
				EnumLineType line_type = table->getLineType(i_line);
				switch (line_type)
				{
				case IT_TITLE:
					table->setTitle(i_line, std_value);
					break;
				case IT_CELL:
					if (table->getCellType(i_line, j_column) == IT_STRING)
						table->setCellText(i_line, j_column, std_value);
					else
						table->setCellLec(i_line, j_column, std_value);
					break;
				default:
					break;
				}
			}
			emit dataChanged(index, index, { role });
			return true;
		}
		catch (const std::runtime_error& e)
		{
			QMessageBox::critical(static_cast<QWidget*>(parent()), tr("ERROR"), tr(e.what()));
		}
	}
	else
	{
		return false;
	}
}

EnumLineType QIodeEditTableModel::getLineType(const int row) const
{
	// first row represents the diviser line
	return row == 0 ? IT_CELL : table->getLineType(row - 1);
}

void QIodeEditTableModel::save()
{	
	KDBTables kdb;
	kdb.update(tableName.toStdString(), *table);
}
