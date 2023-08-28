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
#include "print/print.h"
#include "iode_objs/delegates/base_delegate.h"
#include "iode_objs/edit/edit_vars_sample.h"


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

	QShortcut* sameCmtShortcut;
	QShortcut* sameEqShortcut;
	QShortcut* sameIdtShortcut;
	QShortcut* sameLstShortcut;
	QShortcut* sameTblShortcut;
	QShortcut* varsFromClecShortcut;
	QShortcut* scalarsFromClecShortcut;

	QShortcut* relatedCmtShortcut;
	QShortcut* relatedEqShortcut;
	QShortcut* relatedIdtShortcut;
	QShortcut* relatedLstShortcut;
	QShortcut* relatedSclShortcut;
	QShortcut* relatedTblShortcut;
	QShortcut* relatedVarShortcut;

protected:
	void keyPressEvent(QKeyEvent* event);

	virtual void filter_and_update() = 0;

	/**
	 * @brief Dump displayed table into the document.
	 * 
	 * @note see https://forum.qt.io/post/460045 
	 *       and https://gis.stackexchange.com/q/385616 
	 * 
	 */
	virtual void dumpTableInDocument() = 0;

public:
	AbstractTableView(EnumIodeType iodeType, BaseDelegate* delegate, QWidget* parent = nullptr);
	~AbstractTableView();

	virtual void setup() {}

	virtual void update() {}

	int preferredHeight()
	{
		int height = horizontalHeader()->height();
		for(int i = 0; i < model()->rowCount(); i++)
			height += rowHeight(i);
		if(iodeType == I_VARIABLES)
			height += 20;
		return height;
	}

	void enableDeleteShortcut(bool enable)
	{
		deleteShortcut->setEnabled(enable);
	}

	bool checkGlobalSample()
	{
		// check global sample and ask to set it if not already defined
		KDBVariables kdb;
		if (kdb.get_nb_periods() == 0)
		{
			QWidget* p = static_cast<QWidget*>(parent());
			QMessageBox::StandardButton reply = QMessageBox::question(p, "Sample", "Sample undefined. Set it?");
			if (reply == QMessageBox::Yes)
			{
				QIodeEditVarsSample dialog(this);
				return dialog.exec() == QDialog::Accepted;
			}
			else
				return false;
		}
	}

signals:
	void newObjectInserted();
	void showObjsRequest(EnumIodeType other_type, const QStringList& objNames);

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

	void filter() 
	{ 
		filter_and_update(); 
	}

	virtual void print();

	/**
	 * @brief shows scalars or variables listed in the clec structure or 
	 *        the object of the same name.
	 * 
	 * @param iode_type 
	 */
	virtual void showSameObjOrObjsFromClec(const EnumIodeType iode_type) = 0;

	/**
	 * @brief shows all related objects.
	 * 
	 * @param iode_type 
	 */
	virtual void showRelatedObjs(const EnumIodeType iode_type) = 0;
};


/* NOTE FOR THE DEVELOPERS:
 * Template classes not supported by Q_OBJECT (and then Signals and Slots)
 */


template <class M> class TemplateTableView : public AbstractTableView
{
protected:
	void filter_and_update() override
	{
		QString pattern = filterLineEdit->text().trimmed();
		M* table_model = static_cast<M*>(model());
		table_model->filter(pattern);
		update();
	}

	void dumpTableInDocument() override
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
		: AbstractTableView(iodeType, delegate, parent) 
	{
		setItemDelegate(delegate);
	}

	void setFilterLineEdit(QLineEdit* filterLineEdit)
	{
		this->filterLineEdit = filterLineEdit;
	}

	void update() override
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

	void showSameObjOrObjsFromClec(const EnumIodeType other_type)
	{
		// get the selected object
		QModelIndexList selection = this->selectionModel()->selectedRows();
		if(selection.size() == 0) 
			return;
		QModelIndex index = selection[0];

		// get name of the selected object
		M* table_model = static_cast<M*>(model());		
		QString name = table_model->headerData(index.row(), Qt::Vertical, Qt::DisplayRole).toString();

		// get the list of other objects og type other_type of the same name or present in the CLEC structure
		QStringList list = table_model->getSameObjOrObjsFromClec(name, other_type);

		emit showObjsRequest(other_type, list);
	}

	void showRelatedObjs(const EnumIodeType other_type)
	{
		// get the selected object
		QModelIndexList selection = this->selectedIndexes();
		if(selection.size() == 0) 
			return;
		QModelIndex index = selection[0];

		// get name of the selected object
		M* table_model = static_cast<M*>(model());		
		QString name = table_model->headerData(index.row(), Qt::Vertical, Qt::DisplayRole).toString();

		// get the list of all related other objects of type other_type
		QStringList list = table_model->getRelatedObjs(name, other_type);

		emit showObjsRequest(other_type, list);
	}
};
