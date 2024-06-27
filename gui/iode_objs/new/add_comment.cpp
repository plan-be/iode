#include "add_comment.h"


AddCommentDialog::AddCommentDialog(KDBComments* database, QWidget* parent) : QDialog(parent), database(database)
{
	setupUi(this);

	label_title->setText("Add Comment");
	label_definition->setText("Comment");

	lineName = new WrapperIodeNameEdit(label_name->text(), *lineEdit_name, COMMENTS, REQUIRED_FIELD);
	lineDefinition = new WrapperQLineEdit(label_definition->text(), *lineEdit_definition, REQUIRED_FIELD);
}

void AddCommentDialog::add()
{
	try
	{
		std::string name = lineName->extractAndVerify().toStdString();
		std::string comment = lineDefinition->extractAndVerify().toStdString();
		// check if already exists in the global IODE database since the variable 'database' may represents only a subset
		if(Comments.contains(name))
		{
			int res = QMessageBox::question(nullptr, "WARNING", QString::fromStdString(name) + " already exists. Replace it ?");
			if(res != QMessageBox::StandardButton::Yes)
			{
				this->reject();
				return;
			}
			Comments.update(name, comment);
		}
		else
		{
			emit newObjectInserted(QString::fromStdString(name));
			database->add(name, comment);
		}
		
		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}

void AddCommentDialog::help()
{
	
	QDesktopServices::openUrl(url_manual);
}
