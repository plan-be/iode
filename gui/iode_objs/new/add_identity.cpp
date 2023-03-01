#include "add_identity.h"


QIodeAddIdentity::QIodeAddIdentity(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);

	label_title->setText("Add Identity");
	label_definition->setText("Identity");

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, I_IDENTITIES, REQUIRED_FIELD);
	lineDefinition = new WrapperQLineEdit(label_definition->text(), *lineEdit_definition, REQUIRED_FIELD);
}

void QIodeAddIdentity::add()
{
	try
	{
		KDBIdentities kdb;
		std::string name = lineName->extractAndVerify().toStdString();
		std::string identity = lineDefinition->extractAndVerify().toStdString();
		kdb.add(name, identity);
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}
}

void QIodeAddIdentity::help()
{
	
	QDesktopServices::openUrl(url_manual);
}

