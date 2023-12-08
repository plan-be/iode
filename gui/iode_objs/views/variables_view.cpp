#include "variables_view.h"


void VariablesView::print()
{
	try
	{
		QSettings* project_settings = ProjectSettings::getProjectSettings();
		bool printToFile = project_settings->value(MenuFilePrintSetup::KEY_SETTINGS_PRINT_DEST).toBool();

		if(printToFile)
		{
			VariablesModel* model_ = static_cast<VariablesModel*>(model());

			// set the output file to the filepath associated with the Variable objects 
			// without the extension
			QString filepath = model_->getFilepath();
			QFileInfo fileInfo(filepath);
			QString outputFile = fileInfo.absoluteDir().filePath(fileInfo.completeBaseName());

			QChar format;

			// ask the user to set the output file and format
			PrintFileDialog dialog(this, outputFile);
			if(dialog.exec() == QDialog::Accepted)
			{
				outputFile = dialog.getOutputFile();
				format = dialog.getFormat();
			}
			else
				return;

			// set the number of decimals
			int NbDecimals = model_->get_precision();

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

	AddVariableDialog dialog(this);
	if(dialog.exec() == QDialog::Accepted)
		emit newObjectInserted();
	filter();
}

void VariablesView::plot_series()
{
	try
	{
		QStringList variableNames = extractObjectsNames();
		QStringList sample = extractSample();
		QString from = sample.first();
		QString to = sample.last();

		PlotVariablesDialog* plotDialog = new PlotVariablesDialog();

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
