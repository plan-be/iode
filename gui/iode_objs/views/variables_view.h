#pragma once

#include <QObject>
#include <QMessageBox>

#include "abstract_table_view.h"
#include "iode_objs/models/variables_model.h"
#include "iode_objs/delegates/variables_delegate.h"
#include "iode_objs/new/add_variable.h"


class VariablesView : public AbstractTableView<VariablesModel>
{
	Q_OBJECT

protected:
	void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;

public:
	VariablesView(QWidget* parent = nullptr) : AbstractTableView(I_VARIABLES, new VariablesDelegate(parent), parent) 
	{
		// ---- Selection ----
		// See: - https://doc.qt.io/qt-5/model-view-programming.html#handling-selections-in-item-views
		//      - https://doc.qt.io/qt-5/model-view-programming.html#selections
		
		// -- Behavior --
		// Selecting single items.
		// See: https://doc.qt.io/qt-5/qabstractitemview.html#SelectionBehavior-enum
		setSelectionBehavior(QTableView::SelectItems);
		// -- Mode --
		// When the user selects an item in the usual way, the selection is cleared and the new item selected. 
		// However, if the user presses the Shift key while clicking on an item, all items between the current 
		// item and the clicked item are selected or unselected, depending on the state of the clicked item.
		// See: https://doc.qt.io/qt-5/qabstractitemview.html#SelectionMode-enum
		setSelectionMode(QTableView::ContiguousSelection);
	};

public slots:
	void filter() { filter_and_update(); }
	void new_obj();
};
