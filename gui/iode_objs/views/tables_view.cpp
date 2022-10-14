#include "tables_view.h"


void TablesView::new_obj()
{
	QIodeAddTable dialog(*project_settings_filepath, this);
	dialog.exec();
	filter_and_update();
}
