#include "add_identity.h"


AddIdentityDialog::AddIdentityDialog(KDBIdentities* database, QWidget* parent) : QDialog(parent), database(database)
{
	setupUi(this);

	label_title->setText("Add Identity");
	label_definition->setText("Identity");

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, IDENTITIES, REQUIRED_FIELD);
	lineDefinition = new WrapperQLineEdit(label_definition->text(), *lineEdit_definition, REQUIRED_FIELD);
}

void AddIdentityDialog::add()
{
	try
	{
		std::string name = lineName->extractAndVerify().toStdString();
		std::string identity = lineDefinition->extractAndVerify().toStdString();
		// check if already exists in the global IODE database since the variable 'database' may represents only a subset
		if(Identities.contains(name))
		{
			int res = QMessageBox::question(nullptr, "WARNING", QString::fromStdString(name) + " already exists. Replace it ?");
			if(res != QMessageBox::StandardButton::Yes)
			{
				this->reject();
				return;
			}
			Identities.update(name, identity);
		}
		else
		{
			emit newObjectInserted(QString::fromStdString(name));
			database->add(name, identity);
		}
		
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

