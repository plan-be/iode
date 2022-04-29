#include "add_identity.h"


QIodeAddIdentity::QIodeAddIdentity(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);

	titleLabel->setText("Add Identity");
	definitionLabel->setText("Identity");
}

void QIodeAddIdentity::add()
{
	try
	{
		KDBIdentities kdb;
		std::string name = nameLineEdit->text().toStdString();
		std::string identity = definitionLineEdit->text().toStdString();
		kdb.add(name, identity);
		this->accept();
	}
	catch (const std::runtime_error& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}
}
