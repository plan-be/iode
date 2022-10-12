#include "comments_view.h"


void CommentsView::new_obj()
{
	QIodeAddComment dialog(this);
	dialog.exec();
	filter_and_update();
}
