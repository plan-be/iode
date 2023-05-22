#include "variables_view.h"


void VariablesView::print()
{
	try
	{
		QSettings* project_settings = QIodeProjectSettings::getProjectSettings();
		bool printToFile = project_settings->value(QIodeMenuFilePrintSetup::KEY_SETTINGS_PRINT_DEST).toBool();

		if(printToFile)
		{
			// ask the user to set the output file and format
			QIodePrintFileDialog dialog(this);
			if(dialog.exec() == QDialog::Rejected)
				return;

			// extract the output file
			QString outputFile = project_settings->value(QIodePrintFileDialog::KEY_SETTINGS_PRINT_OUTPUT_FILE).toString();
		
			// extract the format of the output file
			QChar format = QIodePrintFileDialog::getFormat(project_settings);

			// set the number of decimals
			int NbDecimals = 2;

			// set the language
			EnumLang lang = EnumLang::IT_ENGLISH;

			// build the generalized sample
			KDBVariables kdb_var;
			Sample smpl = kdb_var.get_sample();
			QString gsample = QString::fromStdString(smpl.start_period().to_string()) + ":" + QString::number(smpl.nb_periods());

			// list of names = filter pattern or * if pattern is empty
			QString pattern = filterLineEdit->text().trimmed();
			QString names = pattern.isEmpty() ? "*" : pattern;

			// empty files list
			QStringList files;

			printVariableToFile(outputFile, format, names, gsample, files, NbDecimals, lang);
		}
		else
		{
			QPrintPreviewDialog dialog(&printer);
			connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &VariablesView::renderForPrinting);
			dumpTableInDocument();
			dialog.exec();
		}
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString::fromStdString(e.what()));
	}
}

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
	try
	{
		QList<QString> variableNames = extractVariablesNamesFromTo();
		QString to = variableNames.takeLast();
		QString from = variableNames.takeLast();

		QIodePlotVariablesDialog* plotDialog = new QIodePlotVariablesDialog();

		KDBVariables kdb_var;
		plotDialog->setPeriods(kdb_var.get_sample(), from, to);

		foreach(const QString& variable, variableNames)
			plotDialog->addSeries(variable);

		plotDialog->plot();
		emit newPlot(plotDialog);
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}

void VariablesView::open_graphs_dialog()
{
	try
	{
		QList<QString> variableNames = extractVariablesNamesFromTo();
		QString to = variableNames.takeLast();
		QString from = variableNames.takeLast();

		emit newGraphsDialog(variableNames, from, to);
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}
