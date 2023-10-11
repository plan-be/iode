#include "edit_iode_obj.h"


EditObjectDialog::EditObjectDialog(const EnumIodeType iodeType, const QString& objectName, QWidget* parent) : 
	QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint), 
    iodeType(iodeType), objectName(objectName), completer(nullptr)
{
	setupUi(this);

    setWindowTitle("Edititing " + QString::fromStdString(vIodeTypes[iodeType]) + " " + objectName);
    
	textEdit_content->setLineWrapMode(QPlainTextEdit::LineWrapMode::WidgetWidth);

    fullScreenShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_X), this);
    fullScreenShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);

    connect(fullScreenShortcut, &QShortcut::activated, this, &EditObjectDialog::showMaximized);
}

EditObjectDialog::~EditObjectDialog()
{
	delete fullScreenShortcut;

    if(completer) 
        delete completer;
}

void EditObjectDialog::help()
{
    QDesktopServices::openUrl(url_manual);
}


EditCommentDialog::EditCommentDialog(const QString& objectName, QWidget* parent):
    EditObjectDialog(I_COMMENTS, objectName, parent) 
{
    try
    {
        std::string comment = Comments.get(objectName.toStdString());
        textEdit_content->setPlainText(QString::fromStdString(comment));
        computeHash(comment, true);
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
    }
}

void EditCommentDialog::edit()
{
    try
    {
        Comment comment = textEdit_content->toPlainText().toStdString();
        Comments.update(objectName.toStdString(), comment);
        computeHash(comment);

		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}


EditIdentityDialog::EditIdentityDialog(const QString& objectName, QWidget* parent):
    EditObjectDialog(I_IDENTITIES, objectName, parent) 
{
    try
    {
        completer = new IodeCompleter(false, false, {I_SCALARS, I_VARIABLES}, textEdit_content);
        textEdit_content->setCompleter(completer);

        std::string lec = Identities.get_lec(objectName.toStdString());
        textEdit_content->setPlainText(QString::fromStdString(lec));
        computeHash(lec, true);
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
    }
}


void EditIdentityDialog::edit()
{
    try
    {
        std::string lec = textEdit_content->toPlainText().toStdString();
        Identities.update(objectName.toStdString(), lec);
        computeHash(lec);

		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}


EditListDialog::EditListDialog(const QString& objectName, QWidget* parent):
    EditObjectDialog(I_LISTS, objectName, parent) 
{
    try
    {
        completer = new IodeCompleter(false, false, {I_SCALARS, I_VARIABLES}, textEdit_content);
        textEdit_content->setCompleter(completer);

        std::string list = Lists.get(objectName.toStdString());
        textEdit_content->setPlainText(QString::fromStdString(list));
        computeHash(list, true);
    }
    catch(const std::exception& e)
    {
        QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
    }     
}

void EditListDialog::edit()
{
    try
    {
        List list = textEdit_content->toPlainText().toStdString();
        Lists.update(objectName.toStdString(), list);
        computeHash(list);

		this->accept();
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}
}
