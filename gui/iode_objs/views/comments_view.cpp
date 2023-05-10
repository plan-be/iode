#include "comments_view.h"


void CommentsView::print()
{
	QPrintPreviewDialog dialog(&printer);
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &CommentsView::renderForPrinting);
	dumpTableInDocument();
	dialog.exec();
}

void CommentsView::new_obj()
{
	QIodeAddComment dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter_and_update();
}
