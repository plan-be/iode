#pragma once

#include <QObject>
#include <QMessageBox>
#include <QShortcut>

#include "main_window_abstract.h"
#include "iode_objs/models/variables_model.h"
#include "iode_objs/delegates/variables_delegate.h"
#include "iode_objs/new/add_variable.h"
#include "plot/plot_vars.h"
#include "print/print_file_dialog.h"
#include "menu/print_graph/graph_variables.h"
#ifndef TEMPLATE_NUMERICAL_VIEW_HEADER
#include "numerical_view.h"
#endif


class VariablesView : public TemplateNumericalTableView<VariablesModel>
{
	Q_OBJECT

	QShortcut* plotSeriesShortcut;
	QShortcut* graphsDialogShortcut;

public:
	VariablesView(QWidget* parent = nullptr) 
		: TemplateNumericalTableView(I_VARIABLES, new VariablesDelegate(parent), parent) 
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

		setupContextMenu();
	};

	~VariablesView()
	{
		delete plotSeriesShortcut;
		delete graphsDialogShortcut;
	}

	void setup() override
	{	
		MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
		connect(this, &VariablesView::newPlot, main_window, &MainWindowAbstract::appendDialog);
    	connect(this, &VariablesView::newGraphsDialog, main_window, 
        	&MainWindowAbstract::open_graphs_variables_dialog_from_vars_view);
	}

	void setupContextMenu()
    {
        QAction* action;

        action = addAction("Plot", "Plot selected variables", plotSeriesShortcut->key());
        QObject::connect(action, &QAction::triggered, this, &VariablesView::plot_series);

        action = addAction("Plot dialog", "Open the plot dialog", graphsDialogShortcut->key());
        QObject::connect(action, &QAction::triggered, this, &VariablesView::open_graphs_dialog);
	}

signals:
	void newPlot(QDialog* plotDialog);
	void newGraphsDialog(const QList<QString>& variableNames, const QString& from, const QString& to);

public slots:
	void print() override;
	void new_obj();
	void plot_series();
	void open_graphs_dialog();
};
