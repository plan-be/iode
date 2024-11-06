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
#include "iode_objs/models/abstract_table_model.h"
#include "iode_objs/delegates/base_delegate.h"
#include "iode_objs/edit/edit_vars_sample.h"


class IodeAbstractTableView : public QTableView
{
	Q_OBJECT

protected:
	IodeTypes iodeType;
	BaseDelegate* delegate;
	QLineEdit* filterLineEdit;

	int sectionEdited;
	QLineEdit* objectNameEdit;

	QPrinter printer;
	QTextDocument document;

	QShortcut* deleteShortcut;

	QShortcut* shortcutEditEnter;
	QShortcut* shortcutEditReturn;

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

signals:
	void databaseModified();
	void showObjsRequest(IodeTypes other_type, const QStringList& objNames);

public:
	IodeAbstractTableView(IodeTypes iodeType, BaseDelegate* delegate, QWidget* parent = nullptr);
	~IodeAbstractTableView();

	virtual void setup() {}

	virtual void update()
	{
		IodeAbstractTableModel* table_model = static_cast<IodeAbstractTableModel*>(model());
		table_model->reset();
	}

	void setFilterLineEdit(QLineEdit* filterLineEdit)
	{
		this->filterLineEdit = filterLineEdit;
	}

	int preferredHeight()
	{
		int height = horizontalHeader()->height();
		for(int i = 0; i < model()->rowCount(); i++)
			height += rowHeight(i);
		if(iodeType == VARIABLES)
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
		if (Variables.get_nb_periods() == 0)
		{
			QMessageBox::StandardButton reply = QMessageBox::question(nullptr, "Sample", "Sample undefined. Set it?");
			if (reply == QMessageBox::Yes)
			{
				try
				{
					EditIodeSampleDialog dialog(this);
					if (dialog.exec() == QDialog::Accepted)
					{
						std::string from = dialog.get_from().toStdString();
						std::string to = dialog.get_to().toStdString();
						Variables.set_sample(from, to);
					}
					return true;
				}
				catch(const std::exception& e)
				{
					QMessageBox::warning(nullptr, tr("WARNING"), tr(e.what()));
					return false;
				}
			}
			else
				return false;
		}
	}

	void filter(const bool silent = false) 
	{ 
		QString pattern = filterLineEdit->text().trimmed();
		IodeAbstractTableModel* table_model = static_cast<IodeAbstractTableModel*>(model());
		table_model->filter(pattern, silent);
		update();
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

protected:
	void keyPressEvent(QKeyEvent* event);

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

		IodeAbstractTableModel* table_model = static_cast<IodeAbstractTableModel*>(model());
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
					text = table_model->data(table_model->index(row, data_col), Qt::DisplayRole).toString();
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
	
	virtual void new_obj()  { }
	virtual void edit_obj() { }

	void newObjectInserted(const QString& name)
	{
		IodeAbstractTableModel* table_model = static_cast<IodeAbstractTableModel*>(model());
		if(table_model->filterActive())
		{
			QString pattern = filterLineEdit->text().trimmed();
			pattern += ";" + name;
			filterLineEdit->setText(pattern);
		}
	}

	void objectRemoved(const QString& name)
	{
		IodeAbstractTableModel* table_model = static_cast<IodeAbstractTableModel*>(model());
		if(table_model->filterActive())
		{
			QString pattern = filterLineEdit->text().trimmed();
			if(pattern.contains(";" + name))
				pattern.remove(";" + name);
			filterLineEdit->setText(pattern);
		}
	}

	void filter_slot() 
	{ 
		filter();
	}

	virtual void print();

	/**
	 * @brief shows equations, identities, scalars or variables listed in the clec 
	 *        structure or the object of the same name.
	 * 
	 * @param iode_type 
	 */
	void showSameNameObjOrObjsFromClec(const IodeTypes other_type)
	{
		// get the selected object
		QModelIndexList selection = this->selectionModel()->selectedRows();
		if(selection.size() == 0) 
			return;
		QModelIndex index = selection[0];

		// get name of the selected object
		IodeAbstractTableModel* table_model = static_cast<IodeAbstractTableModel*>(model());		
		QString name = table_model->headerData(index.row(), Qt::Vertical, Qt::DisplayRole).toString();

		// get the list of other objects og type other_type of the same name or present in the CLEC structure
		QStringList list = table_model->getSameNameObjOrObjsFromClec(name, other_type);

		emit showObjsRequest(other_type, list);
	}

	/**
	 * @brief shows all related objects.
	 * 
	 * @param iode_type 
	 */
	void showRelatedObjs(const IodeTypes other_type)
	{
		// get the selected object
		QModelIndexList selection = this->selectedIndexes();
		if(selection.size() == 0) 
			return;
		QModelIndex index = selection[0];

		// get name of the selected object
		IodeAbstractTableModel* table_model = static_cast<IodeAbstractTableModel*>(model());		
		QString name = table_model->headerData(index.row(), Qt::Vertical, Qt::DisplayRole).toString();

		// get the list of all related other objects of type other_type
		QStringList list = table_model->getRelatedObjs(name, other_type);

		emit showObjsRequest(other_type, list);
	}
};


template <class M, class D> class TableViewEditObj
{
protected:
	IodeAbstractTableView* tableview;
	D* editDialog;

public:
	TableViewEditObj(IodeAbstractTableView* tableview) : 
		tableview(tableview), editDialog(nullptr) { }

	~TableViewEditObj()
	{
		if(editDialog)
		{
			editDialog->close();
			delete editDialog;
		}
	}

	void openEditDialog()
	{
		QModelIndexList selection = tableview->selectionModel()->selectedRows();
		if (selection.count() == 0) 
			return;

		int section = selection[0].row();
		QString name = tableview->model()->headerData(section, Qt::Vertical).toString();
		
		if(editDialog)
		{
			editDialog->close();
			delete editDialog;
		}

		M* model_ = static_cast<M*>(tableview->model());
		editDialog = new D(name, model_->get_displayed_database(), static_cast<QWidget*>(tableview));

		// propagate signal
		QObject::connect(editDialog, &D::databaseModified, tableview, &IodeAbstractTableView::databaseModified);
		editDialog->show();
	}
};

template <class M, class A> class TableViewAddObj
{
	IodeAbstractTableView* tableview;

public:
	TableViewAddObj(IodeAbstractTableView* tableview) : tableview(tableview) { }

protected:
	void openAddDialog()
	{
		M* model_ = static_cast<M*>(tableview->model());
		A dialog(model_->get_displayed_database(), static_cast<QWidget*>(tableview));
		QObject::connect(&dialog, &A::newObjectInserted, tableview, &IodeAbstractTableView::newObjectInserted);
		if(dialog.exec() == QDialog::Accepted)
			model_->reset();
	}
};
