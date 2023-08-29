#include "add_variable.h"


AddVariableDialog::AddVariableDialog(QWidget* parent) : QDialog(parent)
{
	setupUi(this);

	label_title->setText("Add Variable");
	label_definition->setText("LEC");

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, I_VARIABLES, REQUIRED_FIELD);
	lineDefinition = new WrapperQLineEdit(label_definition->text(), *lineEdit_definition, REQUIRED_FIELD);
}

void AddVariableDialog::add()
{
	try
	{
		KDBVariables kdb;
		std::string name = lineName->extractAndVerify().toStdString();
		std::string lec = lineDefinition->extractAndVerify().toStdString();
		kdb.add(name, lec);
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("WARNING"), tr(e.what()));
	}
}

void AddVariableDialog::help()
{
	
	QDesktopServices::openUrl(url_manual);
}
