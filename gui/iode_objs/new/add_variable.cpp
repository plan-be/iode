#include "add_variable.h"


AddVariableDialog::AddVariableDialog(KDBVariables* database, QWidget* parent) : QDialog(parent), database(database)
{
	setupUi(this);

	label_title->setText("Add Variable");
	label_definition->setText("LEC");

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, VARIABLES, REQUIRED_FIELD);
	lineDefinition = new WrapperQLineEdit(label_definition->text(), *lineEdit_definition, REQUIRED_FIELD);
}

void AddVariableDialog::add()
{
	try
	{
		std::string name = lineName->extractAndVerify().toStdString();
		std::string lec = lineDefinition->extractAndVerify().toStdString();
		// check if already exists in the global IODE database since the variable 'database' may represents only a subset
		if(Variables.contains(name))
		{
			int res = QMessageBox::question(nullptr, "WARNING", QString::fromStdString(name) + " already exists. Replace it ?");
			if(res != QMessageBox::StandardButton::Yes)
			{
				this->reject();
				return;
			}
			Variables.update(name, lec);
		}
		else
		{
			emit newObjectInserted(QString::fromStdString(name));
			database->add(name, lec);
		}
		
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}

void AddVariableDialog::help()
{
	
	QDesktopServices::openUrl(url_manual);
}
