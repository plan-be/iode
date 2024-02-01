#include "equations_view.h"


void EquationsView::new_obj()
{
	EquationsModel* model_ = static_cast<EquationsModel*>(model());
	EditEquationDialog dialog("", model_->get_displayed_database(), this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter();
}

