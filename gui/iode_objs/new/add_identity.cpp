#include "add_identity.h"


AddIdentityDialog::AddIdentityDialog(QWidget* parent) : QDialog(parent)
{
	setupUi(this);

	label_title->setText("Add Identity");
	label_definition->setText("Identity");

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, I_IDENTITIES, REQUIRED_FIELD);
	lineDefinition = new WrapperQLineEdit(label_definition->text(), *lineEdit_definition, REQUIRED_FIELD);
}

void AddIdentityDialog::add()
{
	try
	{
		std::string name = lineName->extractAndVerify().toStdString();
		std::string identity = lineDefinition->extractAndVerify().toStdString();
		Identities.add(name, identity);
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}

void AddIdentityDialog::help()
{
	
	QDesktopServices::openUrl(url_manual);
}

