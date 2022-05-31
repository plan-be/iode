#include "tables_view.h"


void TablesView::new_table()
{
	QIodeAddTable dialog(*settings_filepath, this);
	dialog.exec();
	update();
}
