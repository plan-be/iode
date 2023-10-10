#pragma once

#include <QObject>
#include <QModelIndexList>

#include "abstract_table_view.h"
#include "iode_objs/models/equations_model.h"
#include "iode_objs/delegates/equations_delegate.h"
#include "iode_objs/models/equations_model.h"
#include "iode_objs/edit/edit_equation.h"


class EquationsView : public IodeTemplateTableView<EditEquationDialog>
{
	Q_OBJECT

public:
	EquationsView(QWidget* parent = nullptr) : IodeTemplateTableView(I_EQUATIONS, new EquationsDelegate(parent), parent) {}

public slots:
	void new_obj();
	void edit_obj() { openEditDialog(); }
};
