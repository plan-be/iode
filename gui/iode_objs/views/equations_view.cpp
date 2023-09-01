#include "equations_view.h"


void EquationsView::new_obj()
{
	EditEquationDialog dialog("", this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter();
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
	editEqDialog = new EditEquationDialog(name, static_cast<QWidget*>(this->parent()));

	// propagate signal
	connect(editEqDialog, &EditEquationDialog::equationModified, this, &EquationsView::equationModified);

	editEqDialog->show();
	
	update();
}
