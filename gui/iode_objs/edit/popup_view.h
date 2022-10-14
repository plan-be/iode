#pragma once

#include "iode_objs/views/abstract_table_view.h"
#include "iode_objs/delegates/abstract_delegate.h"


/* NOTE FOR THE DEVELOPERS:
 * Template classes not supported by Q_OBJECT (and then Signals and Slots) 
 */


template <class M, class D> class PopupView : public AbstractTableView<M>
{
public:
	PopupView(EnumIodeType iodeType, AbstractDelegate* delegate, QWidget* parent = nullptr) : AbstractTableView<M>(iodeType, delegate, parent) {}

protected:
	void open_popup_edit_window(const QModelIndex& index)
	{
		if (index.column() > 0)
		{
			// use this pointer to avoid "function declaration must be available 
			// as none of the arguments depend on a template parameter" and 
			// "'project_settings_filepath': undeclared identifier" errors.
			// see https://stackoverflow.com/a/15531940

			M* _model = static_cast<M*>(this->model());
			QString name = _model->data(_model->index(index.row(), 0)).toString();
			D dialog(name, *this->project_settings_filepath, static_cast<QWidget*>(this->parent()));
			dialog.exec();
			this->update();
		}
	}
};
