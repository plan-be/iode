#include "equations_view.h"


void EquationsView::print()
{
	QPrintPreviewDialog dialog(&printer);
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &EquationsView::renderForPrinting);
	dumpTableInDocument();
	dialog.exec();
}

void EquationsView::new_obj()
{
	QIodeEditEquation dialog("", this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter_and_update();
}

void EquationsView::edit_obj() 
{  
	QModelIndexList selection = selectionModel()->selectedRows();
	if (selection.count() == 0) 
		return;

	int section = selection[0].row();
	QString name = model()->headerData(section, Qt::Vertical).toString();
	
	if(editEqDialog)
	{
		editEqDialog->close();
		delete editEqDialog;
	}
	editEqDialog = new QIodeEditEquation(name, static_cast<QWidget*>(this->parent()));
	editEqDialog->show();
	
	update();
}
