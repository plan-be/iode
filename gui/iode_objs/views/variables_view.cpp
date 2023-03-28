#include "variables_view.h"


void VariablesView::new_obj()
{
	// check variables sample and ask to set it if not already defined
	KDBVariables kdb;
	if (kdb.get_nb_periods() == 0)
	{
		QWidget* p = static_cast<QWidget*>(parent());
		QMessageBox::StandardButton reply = QMessageBox::question(p, "Sample", "Sample undefined. Set it?");
		if (reply == QMessageBox::Yes)
		{
			QIodeEditVarsSample dialog(this);
			dialog.exec();
		}
		else
			return;
	}

	QIodeAddVariable dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter_and_update();
}

QList<QString> VariablesView::extractVariablesNamesFromTo()
{
	QModelIndexList selection = this->selectedIndexes();

	int row;
	int minRow = model()->rowCount();
	int maxRow = 0;
	int col;
	int minColumn = model()->columnCount();
	int maxColumn = 0;
	foreach(const QModelIndex& index, selection)
	{
		row = index.row();
		if(row < minRow) minRow = row;
		if(row > maxRow) maxRow = row;
		col = index.column();
		if(col < minColumn) minColumn = col;
		if(col > maxColumn) maxColumn = col;
	}

	VariablesModel* varModel = static_cast<VariablesModel*>(model());
	QList<QString> varsFromTo;

	// list of variables
	for(row = minRow; row <= maxRow; row++) 
		varsFromTo << varModel->headerData(row, Qt::Vertical, Qt::DisplayRole).toString();

	// from
	varsFromTo << varModel->headerData(minColumn, Qt::Horizontal, Qt::DisplayRole).toString();
	// to
	varsFromTo << varModel->headerData(maxColumn, Qt::Horizontal, Qt::DisplayRole).toString();

	return varsFromTo;
}

void VariablesView::plot_series()
{
	QList<QString> variableNames = extractVariablesNamesFromTo();
	QString to = variableNames.takeLast();
	QString from = variableNames.takeLast();

	QIodePlotDialog* plotDialog = new QIodePlotDialog();
	plotDialog->plot(variableNames, from, to);
	emit newPlot(plotDialog);
}

void VariablesView::open_graphs_dialog()
{
	QList<QString> variableNames = extractVariablesNamesFromTo();
	QString to = variableNames.takeLast();
	QString from = variableNames.takeLast();

	emit newGraphsDialog(variableNames, from, to);
}
