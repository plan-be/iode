#pragma once
#include "edit_table_model.h"

EditTableModel::EditTableModel(const QString& tableName, QWidget* parent) : QAbstractTableModel(parent)
{
	try
	{
		table = new Table(tableName.toStdString(), nullptr);
	}
	catch (const std::exception& e)
	{
		table = nullptr;
		QMessageBox::critical(static_cast<QWidget*>(parent), tr("ERROR"), tr(e.what()));
	}
}

EditTableModel::~EditTableModel()
{
	if(table) delete table;
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
			EnumLineType line_type = table->get_line(section - 1)->get_line_type();
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
	int nb_columns = table->nb_columns();
	TableLine* line = is_divider ? table->get_divider_line() : table->get_line(index.row() - 1);
	EnumLineType line_type = line->get_line_type();
	TableCell* cell = (line_type == IT_TITLE || line_type == IT_CELL) ? line->get_cell(index.column(), nb_columns) : nullptr;

	if (role == Qt::TextAlignmentRole)
	{
		EnumCellAlign cell_align = (cell != nullptr) ? cell->get_align() : EnumCellAlign::IT_LEFT;
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
		if (is_divider) 
			return int(QFont::StyleNormal);
		
		// line of type FILES, MODE, LINE or DATE
		if (!cell) 
			return int(QFont::StyleNormal);
		
		QFont font;
		if (cell->is_bold()) 
			font.setBold(true);
		if (cell->is_italic()) 
			font.setItalic(true);
		if (cell->is_underline()) 
			font.setUnderline(true);
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
		bool display_quotes; 
		switch (line_type)
		{
		case IT_TITLE:
			s_data = QString::fromStdString(cell->get_content(false));
			break;
		case IT_CELL:
			display_quotes = (!is_divider) && cell->get_type() == IT_STRING;
			s_data = QString::fromStdString(cell->get_content(display_quotes));
			break;
		default:
			break;
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
			int nb_columns = table->nb_columns();
			TableLine* line = is_divider ? table->get_divider_line() : table->get_line(index.row() - 1);
			EnumLineType line_type = line->get_line_type();
			TableCell* cell = (line_type == IT_TITLE || line_type == IT_CELL) ? line->get_cell(index.column(), nb_columns) : nullptr;

			if (is_divider)
			{
				if (cell->get_type() == IT_STRING)
					cell->set_text(std_value);
				else
					cell->set_lec(std_value);
			}
			else
			{
				EnumLineType line_type = line->get_line_type();
				switch (line_type)
				{
				case IT_TITLE:
					cell->set_text(std_value);
					break;
				case IT_CELL:
					// When inserting a new line of type IT_CELL, the attribute TCELL::tc_type of cells is undefined!
					// Rule: If the content starts with a double quotes -> we assume it is a string cell. 
					//       Otherwise, it is a LEC cell.
					cell->set_content(std_value);
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

void EditTableModel::appendLine(EnumLineType lineType)
{
	switch(lineType)
	{
	case EnumLineType::IT_CELL:
		// When adding a new line of type IT_CELL, the attribute TCELL::tc_type of cells is undefined!
		table->add_line_with_cells();
		break;
	case EnumLineType::IT_TITLE:
		table->add_title("");
		break;
	case EnumLineType::IT_LINE:
		table->add_line_separator();
		break;
	case EnumLineType::IT_FILES:
		table->add_line_files();
		break;
	case EnumLineType::IT_DATE:
		table->add_line_date();
		break;
	case EnumLineType::IT_MODE:
		table->add_line_mode();
		break;
	default:
		break;
	}

	resetModel();
}

void EditTableModel::insert_line(EnumLineType lineType, const int position, const bool after)
{
	switch(lineType)
	{
	case EnumLineType::IT_CELL:
		// When inserting a new line of type IT_CELL, the attribute TCELL::tc_type of cells is undefined!
		table->insert_line_with_cells(position, after);
		break;
	case EnumLineType::IT_TITLE:
		table->insert_title(position, "", after);
		break;
	case EnumLineType::IT_LINE:
		table->insert_line_separator(position, after);
		break;
	case EnumLineType::IT_FILES:
		table->insert_line_files(position, after);
		break;
	case EnumLineType::IT_DATE:
		table->insert_line_date(position, after);
		break;
	case EnumLineType::IT_MODE:
		table->insert_line_mode(position, after);
		break;
	default:
		break;
	}

	resetModel();
}

bool EditTableModel::removeRows(int position, int rows, const QModelIndex& index)
{
	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	try
	{
		// divider line doesn't count -> real table row = gui table row - 1
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
	// first row represents the divider line
	TableLine* line = (row == 0) ? table->get_divider_line() : table->get_line(row - 1);
	return line->get_line_type();
}
