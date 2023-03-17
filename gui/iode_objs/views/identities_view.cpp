#include "identities_view.h"


void IdentitiesView::new_obj()
{
	QIodeAddIdentity dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter_and_update();
}
