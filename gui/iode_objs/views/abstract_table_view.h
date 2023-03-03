#pragma once

#include <QTableView>
#include <QLineEdit>
#include "utils.h"
#include "iode_objs/delegates/base_delegate.h"


/* NOTE FOR THE DEVELOPERS:
 * Template classes not supported by Q_OBJECT (and then Signals and Slots)
 */


template <class M> class AbstractTableView : public QTableView
{
	EnumIodeType iodeType;
	BaseDelegate* delegate;
	QLineEdit* filterLineEdit;

protected:
	std::shared_ptr<QString> project_settings_filepath;

protected:
	void keyPressEvent(QKeyEvent* event) override
	{
		if (event->key() == Qt::Key_Delete)
		{
			M* table_model = static_cast<M*>(model());
			QModelIndexList selection = selectionModel()->selectedRows();
			// Note: goes in reverse order because each deleted row shift all the next rows
			for (int i = selection.count() - 1; i >= 0; i--) table_model->removeRow(selection.at(i).row(), QModelIndex());
			update();
		}
		else
		{
			QTableView::keyPressEvent(event);
		}
	}

	void filter_and_update()
	{
		QString pattern = filterLineEdit->text().trimmed();
		M* table_model = static_cast<M*>(model());
		table_model->filter(pattern);
		update();
	}

public:
	AbstractTableView(EnumIodeType iodeType, BaseDelegate* delegate, QWidget* parent = nullptr) : QTableView(parent), iodeType(iodeType), delegate(delegate)
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

	void setupView(M* model, QLineEdit* filterLineEdit)
	{
		setModel(model);
		setItemDelegate(delegate);
		this->filterLineEdit = filterLineEdit;
	}

	void setProjectSettingsFilepath(std::shared_ptr<QString>& settings_filepath)
	{
		this->project_settings_filepath = settings_filepath;
	}

	void update()
	{
		M* table_model = static_cast<M*>(model());
		table_model->reset();
	}
};
