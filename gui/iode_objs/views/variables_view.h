#pragma once

#include <QObject>
#include <QMessageBox>
#include <QShortcut>

#include "main_window_plot.h"
#include "abstract_table_view.h"
#include "iode_objs/models/variables_model.h"
#include "iode_objs/delegates/variables_delegate.h"
#include "iode_objs/new/add_variable.h"
#include "plot/plot_vars.h"
#include "custom_widgets/print_file_dialog.h"
#include "menu/print_graph/graph_variables.h"


class VariablesView : public TemplateTableView<VariablesModel>
{
	Q_OBJECT

	QShortcut* plotSeriesShortcut;
	QShortcut* graphsDialogShortcut;

private:
	QList<QString> extractVariablesNamesFromTo();

public:
	VariablesView(QWidget* parent = nullptr) : TemplateTableView(I_VARIABLES, new VariablesDelegate(parent), parent) 
	{
		// ---- Selection ----
		// See: - https://doc.qt.io/qt-5/model-view-programming.html#handling-selections-in-item-views
		//      - https://doc.qt.io/qt-5/model-view-programming.html#selections
		
		// -- Behavior --
		// Selecting single items.
		// See: https://doc.qt.io/qt-5/qabstractitemview.html#SelectionBehavior-enum
		setSelectionBehavior(QTableView::SelectItems);
		// -- Mode --
		// When the user selects an item in the usual way, the selection is cleared and the new item selected. 
		// However, if the user presses the Shift key while clicking on an item, all items between the current 
		// item and the clicked item are selected or unselected, depending on the state of the clicked item.
		// See: https://doc.qt.io/qt-5/qabstractitemview.html#SelectionMode-enum
		setSelectionMode(QTableView::ContiguousSelection);

		// ---- keyboard shortcuts ----
		plotSeriesShortcut = new QShortcut(QKeySequence(Qt::Key_F8), this);
		graphsDialogShortcut = new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F8), this);

		connect(plotSeriesShortcut, &QShortcut::activated, this, &VariablesView::plot_series);
		connect(graphsDialogShortcut, &QShortcut::activated, this, &VariablesView::open_graphs_dialog);
	};

	~VariablesView()
	{
		delete plotSeriesShortcut;
		delete graphsDialogShortcut;
	}

	void setup() override
	{	
		MainWindowPlot* main_window = static_cast<MainWindowPlot*>(get_main_window_ptr());
		connect(this, &VariablesView::newPlot, main_window, &MainWindowPlot::appendDialog);
    	connect(this, &VariablesView::newGraphsDialog, main_window, 
        	&MainWindowPlot::open_graphs_variables_dialog_from_vars_view);
	}

signals:
	void newPlot(QDialog* plotDialog);
	void newGraphsDialog(const QList<QString>& variableNames, const QString& from, const QString& to);

public slots:
	void filter() { filter_and_update(); }
	void print();
	void new_obj();
	void plot_series();
	void open_graphs_dialog();
};
