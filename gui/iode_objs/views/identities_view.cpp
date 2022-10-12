#include "identities_view.h"


void IdentitiesView::new_obj()
{
	QIodeAddIdentity dialog(this);
	dialog.exec();
	filter_and_update();
}
