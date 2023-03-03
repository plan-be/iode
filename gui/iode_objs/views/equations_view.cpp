#include "equations_view.h"


void EquationsView::new_obj()
{
	QIodeEditEquation dialog("", *project_settings_filepath, this);
	dialog.exec();
	filter_and_update();
}

void EquationsView::edit_obj() 
{  
	QModelIndexList selection = selectionModel()->selectedRows();
	if (selection.count() == 0) 
		return;

	int section = selection[0].row();
	QString name = model()->headerData(section, Qt::Vertical).toString();
	
	QIodeEditEquation dialog(name, *this->project_settings_filepath, static_cast<QWidget*>(this->parent()));
	dialog.exec();
	
	update();
}
