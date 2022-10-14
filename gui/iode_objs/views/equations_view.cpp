#include "equations_view.h"


void EquationsView::new_obj()
{
	QIodeEditEquation dialog("", *project_settings_filepath, this);
	dialog.exec();
	filter_and_update();
}
