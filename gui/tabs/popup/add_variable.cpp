#include "add_variable.h"


QIodeAddVariable::QIodeAddVariable(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);

	titleLabel->setText("Add Variable");
	definitionLabel->setText("LEC");
}

void QIodeAddVariable::add()
{
	try
	{
		KDBVariables kdb;
		std::string name = nameLineEdit->text().toStdString();
		std::string lec = definitionLineEdit->text().toStdString();
		// Check if SAMPLE define
		
		// build variable
		
		// add to KDB
		//kdb.add(name, variable);
		this->accept();
	}
	catch (const std::runtime_error& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}
}
