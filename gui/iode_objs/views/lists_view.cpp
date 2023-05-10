#include "lists_view.h"


void ListsView::print()
{
	QPrintPreviewDialog dialog(&printer);
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &ListsView::renderForPrinting);
	dumpTableInDocument();
	dialog.exec();
}

void ListsView::new_obj()
{
	QIodeAddList dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter_and_update();
}
