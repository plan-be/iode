#include "tables_view.h"


void TablesView::print()
{
	QPrintPreviewDialog dialog(&printer);
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &TablesView::renderForPrinting);
	dumpTableInDocument();
	dialog.exec();
}

void TablesView::new_obj()
{
	QIodeAddTable dialog(*project_settings_filepath, this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter_and_update();
}

void TablesView::edit_obj()
{
	QModelIndexList selection = selectionModel()->selectedRows();
	if (selection.count() == 0) 
		return;

	int section = selection[0].row();
	QString name = model()->headerData(section, Qt::Vertical).toString();
	
	QIodeEditTable dialog(name, *this->project_settings_filepath, static_cast<QWidget*>(this->parent()));
	dialog.exec();
	
	update();
}
