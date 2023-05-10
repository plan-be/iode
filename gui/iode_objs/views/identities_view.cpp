#include "identities_view.h"


void IdentitiesView::print()
{
	QPrintPreviewDialog dialog(&printer);
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &IdentitiesView::renderForPrinting);
	dumpTableInDocument();
	dialog.exec();
}

void IdentitiesView::new_obj()
{
	QIodeAddIdentity dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter_and_update();
}
