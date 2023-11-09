#pragma once
#include "edit_table_model.h"

EditTableModel::EditTableModel(const QString& tableName, QWidget* parent) : QAbstractTableModel(parent), tableName(tableName)
{
	try
	{
		table = new Table(tableName.toStdString(), nullptr);
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(static_cast<QWidget*>(parent), tr("ERROR"), tr(e.what()));
	}
}

EditTableModel::~EditTableModel()
{
	delete table;
}

int EditTableModel::rowCount(const QModelIndex& parent) const
{
	// + 1 for the divider line
	return table->nb_lines() + 1;
}

int EditTableModel::columnCount(const QModelIndex& parent) const
{
	return table->nb_columns();
}

Qt::ItemFlags EditTableModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QVariant EditTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
			EnumLineType line_type = table->get_line_type(section - 1);
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

QVariant EditTableModel::data(const QModelIndex& index, int role) const
{
	QString s_data;

	if (!index.isValid())
		return QVariant();

	bool is_divider = index.row() == 0;
	int i_line = index.row() - 1;
	int j_column = index.column();
	EnumLineType line_type = is_divider ? IT_CELL : table->get_line_type(i_line);

	if (role == Qt::TextAlignmentRole)
	{
		EnumCellAttribute cell_align = EnumCellAttribute::IT_LEFT;
		if (line_type == IT_TITLE || line_type == IT_CELL)
		{
			cell_align = is_divider ? table->get_divider_cell_align(j_column) : table->get_cell_align(i_line, j_column);
		}

		switch (cell_align)
		{
		case IT_LEFT:
			return int(Qt::AlignLeft);
		case IT_CENTER:
			return int(Qt::AlignCenter);
		case IT_RIGHT:
			return int(Qt::AlignRight);
		case IT_DECIMAL:
			return int(Qt::AlignRight);
		default:
			return int(Qt::AlignLeft);
		}
	}

	if (role == Qt::FontRole)
	{
		if (is_divider) return int(QFont::StyleNormal);
		if (!(line_type == IT_TITLE || line_type == IT_CELL)) return int(QFont::StyleNormal);
		
		QFont font;
		if (table->is_cell_bold_font(i_line, j_column)) font.setBold(true);
		if (table->is_cell_italic_font(i_line, j_column)) font.setItalic(true);
		if (table->is_cell_underline_font(i_line, j_column)) font.setUnderline(true);
		return font;
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
			s_data = QString::fromStdString(table->get_divider_cell_content(j_column, false));
		}
		else
		{
			bool display_quotes; 
			switch (line_type)
			{
			case IT_TITLE:
				s_data = QString::fromStdString(table->get_title(i_line));
				break;
			case IT_CELL:
				display_quotes = table->get_cell_type(i_line, j_column) == IT_STRING;
				s_data = QString::fromStdString(table->get_cell_content(i_line, j_column, display_quotes));
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

bool EditTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
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
				if (table->get_divider_cell_type(j_column) == IT_STRING)
					table->set_cell_divider_text(j_column, std_value);
				else
					table->set_cell_divider_lec(j_column, std_value);
			}
			else
			{
				EnumLineType line_type = table->get_line_type(i_line);
				switch (line_type)
				{
				case IT_TITLE:
					table->set_title(i_line, std_value);
					break;
				case IT_CELL:
					// When inserting a new line of type IT_CELL, the attribute TCELL::tc_type of cells is undefined!
					// Rule: If the content starts with a double quotes -> we assume it is a string cell. 
					//       Otherwise, it is a LEC cell.
					table->set_cell_content(i_line, j_column, std_value);
					break;
				default:
					break;
				}
			}
			emit dataChanged(index, index, { role });
			return true;
		}
		catch (const std::exception& e)
		{
			QMessageBox::warning(nullptr, "ERROR", QString(e.what()));
		}
	}
	else
	{
		return false;
	}
}

int EditTableModel::appendLine(EnumLineType lineType)
{
	int position = -1;

	switch(lineType)
	{
	case EnumLineType::IT_CELL:
		position = table->add_line_with_cells();
		for(int col=0; col < table->nb_columns(); col++)
			table->set_cell_type(position, col, table->get_divider_cell_type(col));
		break;
	case EnumLineType::IT_TITLE:
		position = table->add_title("");
		break;
	case EnumLineType::IT_LINE:
		position = table->add_line_separator();
		break;
	case EnumLineType::IT_FILES:
		position = table->add_line_files();
		break;
	case EnumLineType::IT_DATE:
		position = table->add_line_date();
		break;
	case EnumLineType::IT_MODE:
		position = table->add_line_mode();
		break;
	default:
		break;
	}

	resetModel();
	return position;
}

int EditTableModel::insert_line(EnumLineType lineType, const int position, const bool after)
{
	int new_position = -1;

	switch(lineType)
	{
	case EnumLineType::IT_CELL:
		// When inserting a new line of type IT_CELL, the attribute TCELL::tc_type of cells is undefined!
		new_position = table->insert_line_with_cells(position, after);
		break;
	case EnumLineType::IT_TITLE:
		new_position = table->insert_title(position, "", after);
		break;
	case EnumLineType::IT_LINE:
		new_position = table->insert_line_separator(position, after);
		break;
	case EnumLineType::IT_FILES:
		new_position = table->insert_line_files(position, after);
		break;
	case EnumLineType::IT_DATE:
		new_position = table->insert_line_date(position, after);
		break;
	case EnumLineType::IT_MODE:
		new_position = table->insert_line_mode(position, after);
		break;
	default:
		break;
	}

	resetModel();
	return new_position;
}

bool EditTableModel::removeRows(int position, int rows, const QModelIndex& index)
{
	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	try
	{
		// diviser line doesn't count -> real table row = gui table row - 1
		for(int row = position; row < position + rows; row++)
			table->delete_line(row - 1);
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
		return false;
	}

	endRemoveRows();
	return true;
}

EnumLineType EditTableModel::get_line_type(const int row) const
{
	// first row represents the diviser line
	return row == 0 ? IT_CELL : table->get_line_type(row - 1);
}

void EditTableModel::save()
{	
	Tables.update(tableName.toStdString(), *table);
}
