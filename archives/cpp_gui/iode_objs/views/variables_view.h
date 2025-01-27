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
#include "numerical_view.h"

/* NOTE FOR THE DEVELOPERS:
 * Multiple Inheritance Requires QObject to Be First
 * If you are using multiple inheritance, moc assumes that the first inherited class
 * is a subclass of QObject. Also, be sure that only the first inherited class is a QObject.
 * https://doc.qt.io/qt-6/moc.html#multiple-inheritance-requires-qobject-to-be-first
 * --> That's why the class below does not inherit from NumericalTableView but contains 
 *     a member of type NumericalTableView
 */


class VariablesView : public IodeAbstractTableView, public TableViewAddObj<VariablesModel, AddVariableDialog>
{
	Q_OBJECT
	NumericalTableView numeric;

	QShortcut* plotSeriesShortcut;
	QShortcut* graphsDialogShortcut;

signals:
	void newPlot(QDialog* plotDialog);
	void newGraphsDialog(const QList<QString>& variableNames, const QString& from, const QString& to);

public:
	VariablesView(QWidget* parent = nullptr) 
		: IodeAbstractTableView(VARIABLES, new VariablesDelegate(parent), parent), TableViewAddObj(this), numeric(true)
	{
		numeric.setup(this);

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

        action = numeric.addAction_("Plot", "Plot selected variables", plotSeriesShortcut->key());
        connect(action, &QAction::triggered, this, &VariablesView::plot_series);

        action = numeric.addAction_("Plot dialog", "Open the plot dialog", graphsDialogShortcut->key());
    	connect(action, &QAction::triggered, this, &VariablesView::open_graphs_dialog);
	}

protected:
	void contextMenuEvent(QContextMenuEvent* event) override
	{
		numeric.popupContextMenu(event);
		event->accept();
	}

public slots:
	void print() override;
	void new_obj() override
	{ 
		if(!checkGlobalSample())
			return;
		openAddDialog(); 
	}
	void plot_series();
	void open_graphs_dialog();
};
