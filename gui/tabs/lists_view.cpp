#include "lists_view.h"


void ListsView::new_list()
{
	QIodeAddList dialog(this);
	dialog.exec();
	update();
}
