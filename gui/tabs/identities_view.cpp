#include "identities_view.h"


void IdentitiesView::new_identity()
{
	QIodeAddIdentity dialog(this);
	dialog.exec();
	update();
}
