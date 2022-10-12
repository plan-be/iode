#include "equations_view.h"


void EquationsView::new_obj()
{
	QIodeEditEquation dialog("", *settings_filepath, this);
	dialog.exec();
	filter_and_update();
}
