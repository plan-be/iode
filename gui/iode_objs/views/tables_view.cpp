#include "tables_view.h"


void TablesView::print()
{
	QPrintPreviewDialog dialog(&printer);
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &TablesView::renderForPrinting);
	dumpTableInDocument();
	dialog.exec();
}

void TablesView::new_obj()
{
	QIodeAddTable dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter_and_update();
}

void TablesView::edit_obj()
{
	QModelIndexList selection = selectionModel()->selectedRows();
	if (selection.count() == 0) 
		return;

	int section = selection[0].row();
	QString name = model()->headerData(section, Qt::Vertical).toString();
	
	QIodeEditTable dialog(name, static_cast<QWidget*>(this->parent()));

	// propagate signal
	connect(&dialog, &QIodeEditTable::tableModified, this, &TablesView::tableModified);
	
	dialog.exec();
	
	update();
}

void TablesView::display()
{
	// get the selected object
	QModelIndexList selection = this->selectionModel()->selectedRows();
	if(selection.size() == 0) 
		return;
	QModelIndex index = selection[0];

	QString tableName = model()->headerData(index.row(), Qt::Vertical, Qt::DisplayRole).toString();

	try
	{
		// check if global sample is defined
		if(!checkGlobalSample())
			return;

		// generates the GSample representing the whole global sample
		KDBVariables kdb_var;
		Sample smpl = kdb_var.get_sample();
		QString gsample = QString::fromStdString(smpl.start_period().to_string()) + ":" + QString::number(smpl.nb_periods());

		// computes and display the selected table
		MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
		GSampleNumericalDialog* view = new GSampleNumericalDialog(tableName, gsample, 4, "", this);
		main_window->appendDialog(view);
	}
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, "WARNING", "Could not compute table " + tableName + ":\n" + QString::fromStdString(e.what()));
        return;
    }
}

void TablesView::plot()
{
	// get the selected object
	QModelIndexList selection = this->selectionModel()->selectedRows();
	if(selection.size() == 0) 
		return;
	QModelIndex index = selection[0];

	QString tableName = model()->headerData(index.row(), Qt::Vertical, Qt::DisplayRole).toString();

	try
	{
		// check if global sample is defined
		if(!checkGlobalSample())
			return;

		// generates the GSample representing the whole global sample
		KDBVariables kdb_var;
		Sample smpl = kdb_var.get_sample();
		QString gsample = QString::fromStdString(smpl.start_period().to_string()) + ":" + QString::number(smpl.nb_periods());

		// computes the tables and generates the associated graph
		MainWindowAbstract* main_window = static_cast<MainWindowAbstract*>(get_main_window_ptr());
		GSampleTable* gSampleTable = new GSampleTable(tableName.toStdString(), gsample.toStdString());
		QIodePlotTableDialog* plotDialog = new QIodePlotTableDialog(gSampleTable);
		main_window->appendPlot(plotDialog);
	}
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, "WARNING", "Could not compute table " + tableName + ":\n" + QString::fromStdString(e.what()));
        return;
    }
}
