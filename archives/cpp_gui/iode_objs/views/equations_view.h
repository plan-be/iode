#pragma once

#include <QObject>
#include <QModelIndexList>

#include "abstract_table_view.h"
#include "iode_objs/models/equations_model.h"
#include "iode_objs/delegates/equations_delegate.h"
#include "iode_objs/models/equations_model.h"
#include "iode_objs/edit/edit_equation.h"



class EquationsView : public IodeAbstractTableView, public TableViewEditObj<EquationsModel, EditEquationDialog>
{
	Q_OBJECT

public:
	EquationsView(QWidget* parent = nullptr) : 
		IodeAbstractTableView(EQUATIONS, new EquationsDelegate(parent), parent), 
		TableViewEditObj(this) {}

protected:
	void openAddDialog()
	{
		if(editDialog)
		{
			editDialog->close();
			delete editDialog;
		}

		EquationsModel* model_ = static_cast<EquationsModel*>(model());
		editDialog = new EditEquationDialog("", model_->get_displayed_database(), static_cast<QWidget*>(this->parent()));

		// propagate signal
		connect(editDialog, &EditEquationDialog::newObjectInserted, this, &IodeAbstractTableView::newObjectInserted);
		connect(editDialog, &EditEquationDialog::databaseModified, this, &IodeAbstractTableView::databaseModified);
		connect(editDialog, &EditEquationDialog::databaseModified, this, &EquationsView::updateFilter);
		editDialog->show();
	}

public slots:
	void new_obj()  { openAddDialog(); }
	void edit_obj() { openEditDialog(); }

private slots:
	void updateFilter()
	{
		EquationsModel* model_ = static_cast<EquationsModel*>(model());
		if(model_->filterActive())
			filter();
	}
};
