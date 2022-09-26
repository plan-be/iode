#pragma once

#include <QObject>

#include "iode_objs/edit/popup_view.h"
#include "iode_objs/delegates/equations_delegate.h"
#include "iode_objs/models/equations_model.h"
#include "iode_objs/edit/edit_equation.h"


class EquationsView : public PopupView<EquationsModel, QIodeEditEquation>
{
	Q_OBJECT

public:
	EquationsView(QWidget* parent = nullptr) : PopupView(I_EQUATIONS, new EquationsDelegate(parent), parent)
	{
		connect(this, &EquationsView::activated, this, &EquationsView::popup_edit_window);
	}

public slots:
	void filter() { filter_and_update(); }
	void new_equation();
	void popup_edit_window(const QModelIndex& index) { open_popup_edit_window(index); }
};
