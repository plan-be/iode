#pragma once

#include <QObject>
#include <QShortcut>

#include "main_window_plot.h"
#include "abstract_table_view.h"
#include "iode_objs/models/identities_model.h"
#include "iode_objs/delegates/identities_delegate.h"
#include "iode_objs/new/add_identity.h"


class IdentitiesView : public TemplateTableView<IdentitiesModel>
{
	Q_OBJECT

	QShortcut* executeShortcut;

public:
	IdentitiesView(QWidget* parent = nullptr) : TemplateTableView(I_IDENTITIES, new IdentitiesDelegate(parent), parent) 
	{
		// ---- keyboard shortcuts ----
		executeShortcut = new QShortcut(QKeySequence(Qt::Key_F7), this);
		executeShortcut->setContext(Qt::WidgetWithChildrenShortcut);
	};

	void setup(std::shared_ptr<QString>& settings_filepath) override
	{
		AbstractTableView::setup(settings_filepath);

		MainWindowPlot* main_window = static_cast<MainWindowPlot*>(get_main_window_ptr());
		connect(executeShortcut, &QShortcut::activated, main_window, &MainWindowPlot::open_compute_identities_dialog);
	}

public slots:
	void filter() { filter_and_update(); }
	void print() { QMessageBox::warning(nullptr, "WARNING", "Not yet implemented"); }
	void new_obj();
};
