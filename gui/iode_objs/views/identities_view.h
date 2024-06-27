#pragma once

#include <QObject>
#include <QShortcut>

#include "main_window_abstract.h"
#include "abstract_table_view.h"
#include "iode_objs/models/identities_model.h"
#include "iode_objs/delegates/identities_delegate.h"
#include "iode_objs/new/add_identity.h"
#include "iode_objs/edit/edit_identity.h"


class IdentitiesView : public IodeAbstractTableView, public TableViewEditObj<IdentitiesModel, EditIdentityDialog>, public TableViewAddObj<IdentitiesModel, AddIdentityDialog>
{
	Q_OBJECT

	QShortcut* shortcutExecuteCurrentIdt;
	QShortcut* shortcutExecuteIdts;

public:
	IdentitiesView(QWidget* parent = nullptr) : 
		IodeAbstractTableView(IDENTITIES, new IdentitiesDelegate(parent), parent), 
		TableViewEditObj(this), TableViewAddObj(this) 
	{
		// ---- keyboard shortcuts ----
		shortcutExecuteCurrentIdt = new QShortcut(QKeySequence(Qt::Key_F7), this);
		shortcutExecuteIdts = new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F7), this);

		shortcutExecuteCurrentIdt->setContext(Qt::WidgetWithChildrenShortcut);
		shortcutExecuteIdts->setContext(Qt::WidgetWithChildrenShortcut);
	};

	~IdentitiesView()
	{
		delete shortcutExecuteCurrentIdt;
		delete shortcutExecuteIdts;
	}

	void setup() override
	{
		connect(shortcutExecuteCurrentIdt, &QShortcut::activated, this, &IdentitiesView::executeCurrentIdentity);

		MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
		connect(shortcutExecuteIdts, &QShortcut::activated, main_window, &MainWindowAbstract::open_compute_identities_dialog);
	}

public slots:
	void new_obj()  { openAddDialog(); }
	void edit_obj() { openEditDialog(); }
	void executeCurrentIdentity();
};
