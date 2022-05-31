#include "equations_view.h"


void EquationsView::new_equation()
{
	QIodeEditEquation dialog("", *settings_filepath, this);
	dialog.exec();
	update();
}
