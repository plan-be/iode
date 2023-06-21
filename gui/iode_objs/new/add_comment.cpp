#include "add_comment.h"


QIodeAddComment::QIodeAddComment(QWidget* parent) : QDialog(parent)
{
	setupUi(this);

	label_title->setText("Add Comment");
	label_definition->setText("Comment");

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, I_COMMENTS, REQUIRED_FIELD);
	lineDefinition = new WrapperQLineEdit(label_definition->text(), *lineEdit_definition, REQUIRED_FIELD);
}

void QIodeAddComment::add()
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

void QIodeAddComment::help()
{
	
	QDesktopServices::openUrl(url_manual);
}
