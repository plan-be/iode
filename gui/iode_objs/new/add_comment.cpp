#include "add_comment.h"


AddCommentDialog::AddCommentDialog(QWidget* parent) : QDialog(parent)
{
	setupUi(this);

	label_title->setText("Add Comment");
	label_definition->setText("Comment");

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, I_COMMENTS, REQUIRED_FIELD);
	lineDefinition = new WrapperQLineEdit(label_definition->text(), *lineEdit_definition, REQUIRED_FIELD);
}

void AddCommentDialog::add()
{
	try
	{
		KDBComments kdb;
		std::string name = lineName->extractAndVerify().toStdString();
		std::string comment = lineDefinition->extractAndVerify().toStdString();
		kdb.add(name, comment);
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("WARNING"), tr(e.what()));
	}
}

void AddCommentDialog::help()
{
	
	QDesktopServices::openUrl(url_manual);
}
