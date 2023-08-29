#include "lists_view.h"


void ListsView::new_obj()
{
	AddListDialog dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter_and_update();
}
