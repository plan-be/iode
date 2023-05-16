#pragma once

#include <QPrinter>
#include <QLineEdit>
#include <QShortcut>
#include <QTextTable>
#include <QTableView>
#include <QTextCursor>
#include <QStringList>
#include <QModelIndex>
#include <QHeaderView>
#include <QTextDocument>
#include <QTextTableCell>
#include <QModelIndexList>
#include <QAbstractItemModel>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "utils.h"
#include "util/print.h"
#include "iode_objs/delegates/base_delegate.h"


class AbstractTableView : public QTableView
{
	Q_OBJECT

protected:
	EnumIodeType iodeType;
	BaseDelegate* delegate;
	QLineEdit* filterLineEdit;

	int sectionEdited;
	QLineEdit* objectNameEdit;

	QPrinter printer;
	QTextDocument document;

	QShortcut* deleteShortcut;

protected:
	void keyPressEvent(QKeyEvent* event);

public:
	AbstractTableView(EnumIodeType iodeType, BaseDelegate* delegate, QWidget* parent = nullptr);
	~AbstractTableView();

	virtual void setup() {}

signals:
	void newObjectInserted();

protected slots:
	void renderForPrinting() 
	{
		document.print(&printer);
	}

public slots:
	void removeObjects();
	void openEditorName(int section);
	virtual void editName();
	void closeNameEditor()
	{
		objectNameEdit->setHidden(true);
		objectNameEdit->setText("");
	}
};


/* NOTE FOR THE DEVELOPERS:
 * Template classes not supported by Q_OBJECT (and then Signals and Slots)
 */


template <class M> class TemplateTableView : public AbstractTableView
{
protected:
	void filter_and_update()
	{
		QString pattern = filterLineEdit->text().trimmed();
		M* table_model = static_cast<M*>(model());
		table_model->filter(pattern);
		update();
	}

	/**
	 * @brief Dump displayed table into the document.
	 * 
	 * @note see https://forum.qt.io/post/460045 
	 *       and https://gis.stackexchange.com/q/385616 
	 * 
	 */
	void dumpTableInDocument()
	{
		document.clear();
		const int max_nb_columns = 10;

		M* table_model = static_cast<M*>(model());
		int nb_rows = table_model->rowCount();
		int nb_cols_total = table_model->columnCount();

		int nb_tables =  nb_cols_total / max_nb_columns;
		if(nb_cols_total % max_nb_columns > 0)
			nb_tables++;

		QTextCursor cursor(&document);

		int data_col;
		int nb_cols;
		for(int i_table=0; i_table < nb_tables; i_table++)
		{
			nb_cols = (i_table < nb_tables - 1) ? max_nb_columns : nb_cols_total % max_nb_columns;
			QTextTable* table = cursor.insertTable(nb_rows + 1, nb_cols + 1);
			QTextTableFormat tableFormat = table->format();
			tableFormat.setHeaderRowCount(1);
			tableFormat.setCellPadding(4);
			tableFormat.setCellSpacing(0);
			tableFormat.setBorder(0);
			//if(i_table < nb_tables - 1)
			//	tableFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
			table->setFormat(tableFormat);

			QTextTableCell headerCell;
			QTextTableCell cell;
			QString text;
			QTextCharFormat boldFormat;
			boldFormat.setFontWeight(QFont::Bold);

			// top left cell
			headerCell = table->cellAt(0, 0);
			cursor = headerCell.firstCursorPosition();
			cursor.insertText("Name", boldFormat);
			QTextTableCellFormat cellFormat = headerCell.format().toTableCellFormat();
			cellFormat.setBottomBorderStyle(QTextFrameFormat::BorderStyle_Solid);
			headerCell.setFormat(cellFormat);

			// columns header
			for(int col=0; col < nb_cols; col++)
			{
				data_col = (i_table * max_nb_columns) + col;
				text = table_model->headerData(data_col, Qt::Horizontal, Qt::DisplayRole).toString();
				headerCell = table->cellAt(0, col + 1);
				cursor = headerCell.firstCursorPosition();
				cursor.insertText(text, boldFormat);
				QTextTableCellFormat cellFormat = headerCell.format().toTableCellFormat();
				cellFormat.setBottomBorderStyle(QTextFrameFormat::BorderStyle_Solid);
				headerCell.setFormat(cellFormat);
			}
			// table rows
			for(int row=0; row < nb_rows; row++)
			{
				// row header
				text = table_model->headerData(row, Qt::Vertical, Qt::DisplayRole).toString();
				headerCell = table->cellAt(row + 1, 0);
				cursor = headerCell.firstCursorPosition();
				cursor.insertText(text);

				// row cells content
				for(int col=0; col < nb_cols; col++)
				{
					data_col = (i_table * max_nb_columns) + col;
					text = table_model->dataCell(row, data_col).toString();
					cell = table->cellAt(row + 1, col + 1);
					cursor = cell.firstCursorPosition();
					cursor.insertText(text);
				}
			}
			cursor.movePosition(QTextCursor::End);
			if(i_table < nb_tables - 1)
				cursor.insertText("\n\n");
		}
	}

public:
	TemplateTableView(EnumIodeType iodeType, BaseDelegate* delegate, QWidget* parent = nullptr) 
		: AbstractTableView(iodeType, delegate, parent) {}

	void setupView(M* model, QLineEdit* filterLineEdit)
	{
		setModel(model);
		setItemDelegate(delegate);
		this->filterLineEdit = filterLineEdit;
	}

	void update()
	{
		M* table_model = static_cast<M*>(model());
		table_model->reset();
	}

	QStringList getSelectedObjectsNames() const
	{
		QStringList names;
		QModelIndexList selection = this->selectionModel()->selectedRows();
		if(selection.size() == 0) 
			return names;

		QAbstractItemModel* model_obj = this->model();
		foreach(const QModelIndex& index, selection) 
			names << model_obj->headerData(index.row(), Qt::Vertical, Qt::DisplayRole).toString();
		return names;
	}
};
