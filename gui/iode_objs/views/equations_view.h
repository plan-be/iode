#pragma once

#include <QObject>
#include <QModelIndexList>

#include "abstract_table_view.h"
#include "iode_objs/models/equations_model.h"
#include "iode_objs/delegates/equations_delegate.h"
#include "iode_objs/models/equations_model.h"
#include "iode_objs/edit/edit_equation.h"


class EquationsView : public IodeAbstractTableView
{
	Q_OBJECT

	EditEquationDialog* editEqDialog;

public:
	EquationsView(QWidget* parent = nullptr) : IodeAbstractTableView(I_EQUATIONS, new EquationsDelegate(parent), parent)
	{
		editEqDialog = nullptr;
	}

	~EquationsView()
	{
		if(editEqDialog)
		{
			editEqDialog->close();
			delete editEqDialog;
		}
	}

signals:
	void databaseModified();

public slots:
	void new_obj();
	void edit_obj() override;
};
