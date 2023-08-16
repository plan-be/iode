#include "scalars_view.h"


void ScalarsView::new_obj()
{
	QIodeAddScalar dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter_and_update();
}
