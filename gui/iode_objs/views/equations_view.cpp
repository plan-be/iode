#include "equations_view.h"


void EquationsView::new_obj()
{
	EditEquation dialog("", this);
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
	editEqDialog = new EditEquation(name, static_cast<QWidget*>(this->parent()));

	// propagate signal
	connect(editEqDialog, &EditEquation::equationModified, this, &EquationsView::equationModified);

	editEqDialog->show();
	
	update();
}
