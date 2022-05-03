#include "equations_view.h"


void EquationsView::new_equation()
{
	QIodeEditEquation dialog("", this);
	dialog.exec();
	update();
}
