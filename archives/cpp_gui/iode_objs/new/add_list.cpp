#include "add_list.h"


AddListDialog::AddListDialog(KDBLists* database, QWidget* parent) : QDialog(parent), database(database)
{
	setupUi(this);

	label_title->setText("Add List");
	label_definition->setText("List");

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, LISTS, REQUIRED_FIELD);
	lineDefinition = new WrapperQLineEdit(label_definition->text(), *lineEdit_definition, REQUIRED_FIELD);
}

void AddListDialog::add()
{
	try
	{
		std::string name = lineName->extractAndVerify().toStdString();
		std::string list = lineDefinition->extractAndVerify().toStdString();
		// check if already exists in the global IODE database since the variable 'database' may represents only a subset
		if(Lists.contains(name))
		{
			int res = QMessageBox::question(nullptr, "WARNING", QString::fromStdString(name) + " already exists. Replace it ?");
			if(res != QMessageBox::StandardButton::Yes)
			{
				this->reject();
				return;
			}
			Lists.update(name, list);
		}
		else
		{
			emit newObjectInserted(QString::fromStdString(name));
			database->add(name, list);
		}
		
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}

void AddListDialog::help()
{
	
	QDesktopServices::openUrl(url_manual);
}
