#pragma once

#include "abstract_table_view.h"
#include "abstract_delegate.h"


/* NOTE FOR THE DEVELOPERS:
 * Template classes not supported by Q_OBJECT (and then Signals and Slots) 
 */


template <class M, class D> class PopupView : public AbstractTableView<M>
{
public:
	PopupView(EnumIodeType iodeType, AbstractDelegate* delegate, QWidget* parent = nullptr) : AbstractTableView(iodeType, delegate, parent) {}

protected:
	void open_popup_edit_window(const QModelIndex& index)
	{
		if (index.column() > 0)
		{
			M* _model = static_cast<M*>(model());
			QString name = _model->data(_model->index(index.row(), 0)).toString();
			D dialog(name, static_cast<QWidget*>(parent()));
			dialog.exec();
			this->update();
		}
	}
};