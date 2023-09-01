#include "scalars_view.h"


void ScalarsView::new_obj()
{
	AddScalarDialog dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter();
}
