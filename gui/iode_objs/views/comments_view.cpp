#include "comments_view.h"


void CommentsView::new_obj()
{
	AddCommentDialog dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter();
}
