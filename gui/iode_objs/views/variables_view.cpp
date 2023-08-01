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
	if(!checkGlobalSample())
		return;

	QIodeAddVariable dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter_and_update();
}

void VariablesView::plot_series()
{
	try
	{
		QStringList variableNames = extractObjectsNames();
		QStringList sample = extractSample();
		QString from = sample.first();
		QString to = sample.last();

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
		QStringList variableNames = extractObjectsNames();
		QStringList sample = extractSample();
		QString from = sample.first();
		QString to = sample.last();

		emit newGraphsDialog(variableNames, from, to);
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}
