#include "equations_view.h"


void EquationsView::new_obj()
{
	EditEquationDialog dialog("", this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter();
}

