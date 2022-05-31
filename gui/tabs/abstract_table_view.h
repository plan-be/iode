#pragma once

#include <QTableView>
#include "../utils.h"
#include "abstract_delegate.h"

template <class M> class AbstractTableView : public QTableView
{
	EnumIodeType iodeType;
	AbstractDelegate* delegate;

protected:
	std::shared_ptr<QString> settings_filepath;

protected:
	void keyPressEvent(QKeyEvent* event) override
	{
		if (event->key() == Qt::Key_Delete)
		{
			M* table_model = static_cast<M*>(model());
			QModelIndexList selection = selectionModel()->selectedRows();
			for (const QModelIndex& index : selection) table_model->removeRow(index.row());
			update();
		}
		else
		{
			QTableView::keyPressEvent(event);
		}
	}

public:
	AbstractTableView(EnumIodeType iodeType, AbstractDelegate* delegate, QWidget* parent = nullptr) : QTableView(parent), iodeType(iodeType), delegate(delegate) 
	{
		// ---- Selection ----
		// See: - https://doc.qt.io/qt-5/model-view-programming.html#handling-selections-in-item-views
		//      - https://doc.qt.io/qt-5/model-view-programming.html#selections
		
		// -- Behavior --
		// Selecting only rows.
		// See: https://doc.qt.io/qt-5/qabstractitemview.html#SelectionBehavior-enum
		setSelectionBehavior(QTableView::SelectRows);
		// -- Mode --
		// When the user selects an item in the usual way, the selection is cleared and the new item selected. 
		// However, if the user presses the Ctrl key when clicking on an item, the clicked item gets toggled 
		// and all other items are left untouched. 
		// If the user presses the Shift key while clicking on an item, all items between the current item and 
		// the clicked item are selected or unselected, depending on the state of the clicked item. 
		// Multiple items can be selected by dragging the mouse over them.
		// See: https://doc.qt.io/qt-5/qabstractitemview.html#SelectionMode-enum
		setSelectionMode(QTableView::ExtendedSelection);
	}

	~AbstractTableView() {}

	void setupView(M* model, std::shared_ptr<QString>& settings_filepath)
	{
		setModel(model);
		setItemDelegate(delegate);
		this->settings_filepath = settings_filepath;
	}

	void update()
	{
		M* table_model = static_cast<M*>(model());
		table_model->reset();
		table_model->rowCount() > 0 ? show() : hide();
	}
};
