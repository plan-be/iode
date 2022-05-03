#include "tables_view.h"


void TablesView::new_table()
{
	QIodeAddTable dialog(this);
	dialog.exec();
	update();
}
