#include "lists_view.h"


void ListsView::new_obj()
{
	QIodeAddList dialog(this);
	dialog.exec();
	filter_and_update();
}
