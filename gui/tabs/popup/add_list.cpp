#include "add_list.h"


QIodeAddList::QIodeAddList(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);

	titleLabel->setText("Add List");
	definitionLabel->setText("List");
}

void QIodeAddList::add()
{
	try
	{
		KDBLists kdb;
		std::string name = nameLineEdit->text().toStdString();
		std::string list = definitionLineEdit->text().toStdString();
		kdb.add(name, list);
		this->accept();
	}
	catch (const std::runtime_error& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}
}
