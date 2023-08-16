#pragma once

#include <QObject>
#include <QModelIndexList>

#include "abstract_table_view.h"
#include "iode_objs/models/equations_model.h"
#include "iode_objs/delegates/equations_delegate.h"
#include "iode_objs/models/equations_model.h"
#include "iode_objs/edit/edit_equation.h"


class EquationsView : public TemplateTableView<EquationsModel>
{
	Q_OBJECT

	QIodeEditEquation* editEqDialog;

	QShortcut* editShortcutEnter;
	QShortcut* editShortcutReturn;

public:
	EquationsView(QWidget* parent = nullptr) : TemplateTableView(I_EQUATIONS, new EquationsDelegate(parent), parent)
	{
		editEqDialog = nullptr;
		
		editShortcutEnter = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Enter), this);
		connect(editShortcutEnter, &QShortcut::activated, this, &EquationsView::edit_obj);
		
		editShortcutReturn = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), this);
		connect(editShortcutReturn, &QShortcut::activated, this, &EquationsView::edit_obj);
	}

	~EquationsView()
	{
		if(editEqDialog)
		{
			editEqDialog->close();
			delete editEqDialog;
		}
		delete editShortcutEnter;
		delete editShortcutReturn;
	}

signals:
	void equationModified();

public slots:
	void new_obj();
	void edit_obj();
};
