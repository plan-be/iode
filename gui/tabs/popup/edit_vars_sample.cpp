#include "edit_vars_sample.h"

QIodeEditVarsSample::QIodeEditVarsSample(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);

	sampleFrom = new WrapperSampleEdit(label_from->text(), *sampleEdit_from, REQUIRED_FIELD);
	sampleTo = new WrapperSampleEdit(label_to->text(), *sampleEdit_to, REQUIRED_FIELD);

	Sample sample = kdb_vars.get_sample();
	if (sample.nb_periods() == 0)
	{
		labelTitle->setText("New Variables Sample");
	}
	else
	{
		labelTitle->setText("Edit Variables Sample");
		QString from = QString::fromStdString(sample.start_period().to_string());
		QString to = QString::fromStdString(sample.end_period().to_string());
		sampleFrom->setQValue(from);
		sampleTo->setQValue(to);
	}
}

QIodeEditVarsSample::~QIodeEditVarsSample()
{
	delete sampleFrom;
	delete sampleTo;
}

void QIodeEditVarsSample::edit()
{
	try
	{
		std::string from = sampleFrom->extractAndVerify().toStdString();
		std::string to = sampleTo->extractAndVerify().toStdString();

		kdb_vars.set_sample(from, to);

		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}
}

void QIodeEditVarsSample::help()
{
	QUrl url = get_url_iode_manual();
	QDesktopServices::openUrl(url);
}