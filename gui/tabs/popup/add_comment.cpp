#include "add_comment.h"


QIodeAddComment::QIodeAddComment(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);

	titleLabel->setText("Add Comment");
	definitionLabel->setText("Comment");
}

void QIodeAddComment::add()
{
	try
	{
		KDBComments kdb;
		std::string name = nameLineEdit->text().toStdString();
		std::string comment = definitionLineEdit->text().toStdString();
		kdb.add(name, comment);
		this->accept();
	}
	catch (const std::runtime_error& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}
}