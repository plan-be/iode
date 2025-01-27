#include "edit_vars_sample.h"

EditIodeSampleDialog::EditIodeSampleDialog(QWidget* parent) : QDialog(parent)
{
	setupUi(this);

	sampleFrom = new WrapperSampleEdit(label_from->text(), *sampleEdit_from, REQUIRED_FIELD);
	sampleTo = new WrapperSampleEdit(label_to->text(), *sampleEdit_to, REQUIRED_FIELD);

	Sample* sample = Variables.get_sample();
	if (sample->nb_periods() == 0)
	{
		labelTitle->setText("New Variables Sample");
	}
	else
	{
		labelTitle->setText("Variables Sample");
		QString from = QString::fromStdString(sample->start_period().to_string());
		QString to = QString::fromStdString(sample->end_period().to_string());
		sampleFrom->setQValue(from);
		sampleTo->setQValue(to);
	}
}

EditIodeSampleDialog::~EditIodeSampleDialog()
{
	delete sampleFrom;
	delete sampleTo;
}

void EditIodeSampleDialog::edit()
{
	try
	{
		// raises an error if 'from' or 'to' has a wrong value 
		QString from = sampleFrom->extractAndVerify();
		QString to = sampleTo->extractAndVerify();

		// try to create a sample -> raises an error if something went wrong
		Sample sample(from.toStdString(), to.toStdString());

		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, tr("WARNING"), tr(e.what()));
	}
}

void EditIodeSampleDialog::help()
{
	QDesktopServices::openUrl(url_manual);
}