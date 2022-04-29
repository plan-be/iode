#include "variables_view.h"


void VariablesView::new_variable()
{
	QIodeAddVariable dialog(this);
	dialog.exec();
	update();
}
