#include "scalars_view.h"


void ScalarsView::new_scalar()
{
	QIodeAddScalar dialog(*settings_filepath, this);
	dialog.exec();
	filter_and_update();
}
