#include "tables_view.h"


void TablesView::new_obj()
{
	QIodeAddTable dialog(*settings_filepath, this);
	dialog.exec();
	filter_and_update();
}
