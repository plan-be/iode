#include "scalars_view.h"


void ScalarsView::new_scalar()
{
	QIodeAddScalar dialog(this);
	dialog.exec();
	update();
}
