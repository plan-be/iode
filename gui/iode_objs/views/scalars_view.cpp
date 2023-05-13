#include "scalars_view.h"


void ScalarsView::print()
{
	QPrintPreviewDialog dialog(&printer);
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &ScalarsView::renderForPrinting);
	dumpTableInDocument();
	dialog.exec();
}

void ScalarsView::new_obj()
{
	QIodeAddScalar dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter_and_update();
}
