#pragma once
#include "edit_table_model.h"

EditTableModel::EditTableModel(const QString& tableName, QWidget* parent) : QAbstractTableModel(parent), tableName(tableName)
{
	try
	{
		table = new Table(tableName.toStdString());
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
	return table->nbLines() + 1;
}

int EditTableModel::columnCount(const QModelIndex& parent) const
{
	return table->nbColumns();
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

QVariant EditTableModel::data(const QModelIndex& index, int role) const
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
		EnumCellAttribute cell_align = EnumCellAttribute::IT_LEFT;
		if (line_type == IT_TITLE || line_type == IT_CELL)
		{
			cell_align = is_divider ? table->getDividerCellAlign(j_column) : table->getCellAlign(i_line, j_column);
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
		if (table->isCellBoldFont(i_line, j_column)) font.setBold(true);
		if (table->isCellItalicFont(i_line, j_column)) font.setItalic(true);
		if (table->isCellUnderlineFont(i_line, j_column)) font.setUnderline(true);
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
			s_data = QString::fromStdString(table->getDividerCellContent(j_column, false));
		}
		else
		{
			bool display_quotes; 
			switch (line_type)
			{
			case IT_TITLE:
				s_data = QString::fromStdString(table->getTitle(i_line));
				break;
			case IT_CELL:
				display_quotes = table->getCellType(i_line, j_column) == IT_STRING;
				s_data = QString::fromStdString(table->getCellContent(i_line, j_column, display_quotes));
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
					// When inserting a new line of type IT_CELL, the attribute TCELL::tc_type of cells is undefined!
					// Rule: If the content starts with a double quotes -> we assume it is a string cell. 
					//       Otherwise, it is a LEC cell.
					table->setCellContent(i_line, j_column, std_value);
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
		position = table->addLineWithCells();
		for(int col=0; col < table->nbColumns(); col++)
			table->setCellType(position, col, table->getDividerCellType(col));
		break;
	case EnumLineType::IT_TITLE:
		position = table->addTitle("");
		break;
	case EnumLineType::IT_LINE:
		position = table->addLineSeparator();
		break;
	case EnumLineType::IT_FILES:
		position = table->addLineFiles();
		break;
	case EnumLineType::IT_DATE:
		position = table->addLineDate();
		break;
	case EnumLineType::IT_MODE:
		position = table->addLineMode();
		break;
	default:
		break;
	}

	resetModel();
	return position;
}

int EditTableModel::insertLine(EnumLineType lineType, const int position, const bool after)
{
	int new_position = -1;

	switch(lineType)
	{
	case EnumLineType::IT_CELL:
		// When inserting a new line of type IT_CELL, the attribute TCELL::tc_type of cells is undefined!
		new_position = table->insertLineWithCells(position, after);
		break;
	case EnumLineType::IT_TITLE:
		new_position = table->insertTitle(position, "", after);
		break;
	case EnumLineType::IT_LINE:
		new_position = table->insertLineSeparator(position, after);
		break;
	case EnumLineType::IT_FILES:
		new_position = table->insertLineFiles(position, after);
		break;
	case EnumLineType::IT_DATE:
		new_position = table->insertLineDate(position, after);
		break;
	case EnumLineType::IT_MODE:
		new_position = table->insertLineMode(position, after);
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
			table->deleteLine(row - 1);
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
		return false;
	}

	endRemoveRows();
	return true;
}

EnumLineType EditTableModel::getLineType(const int row) const
{
	// first row represents the diviser line
	return row == 0 ? IT_CELL : table->getLineType(row - 1);
}

void EditTableModel::save()
{	
	Tables.update(tableName.toStdString(), *table);
}
