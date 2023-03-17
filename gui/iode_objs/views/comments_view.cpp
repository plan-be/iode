#include "comments_view.h"


void CommentsView::new_obj()
{
	QIodeAddComment dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter_and_update();
}
