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

void IdentitiesView::executeCurrentIdentity()
{
	// get the selected object
	QModelIndexList selection = this->selectionModel()->selectedRows();
	if(selection.size() == 0) 
		return;
	QModelIndex index = selection[0];

	// check if global sample is defined
	if(!checkGlobalSample())
		return;

	// computes the selected identity + updates other IODE objects tabs is necessary
	MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
	main_window->computeHash(true);
	static_cast<IdentitiesModel*>(model())->executeIdentity(index.row());	
	main_window->update_tab_and_completer();	
}
