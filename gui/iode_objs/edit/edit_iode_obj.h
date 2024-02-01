#pragma once

#include <QWidget>
#include <QString>
#include <QShortcut>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QDesktopServices>

#include <string>

#include "ui_edit_iode_obj.h"
#include "utils.h"
#include "text_edit/completer.h"
#include "iode_objs/models/abstract_table_model.h"

#include "cpp_api/KDB/kdb_comments.h"
#include "cpp_api/KDB/kdb_identities.h"
#include "cpp_api/KDB/kdb_lists.h"


/* NOTE FOR THE DEVELOPERS:
 * This Dialog class MUST call the Q_OBJECT macro at the beginning of the class 
 * to allow slots and signals (see documentation of Qt).
 */


/**
 * @brief Dialog box to edit either Comments or Identities or Lists.
 * 
 */
class EditObjectDialog : public QDialog, public Ui::EditObjectDialog
{
    Q_OBJECT

	QShortcut* fullScreenShortcut;

protected:
    std::string name;
    std::string value;
    EnumIodeType iodeType;

    IodeCompleter* completer;

signals:
	void databaseModified();

public:
    EditObjectDialog(const EnumIodeType iodeType, const QString& name, QWidget* parent = Q_NULLPTR);
    ~EditObjectDialog();

protected:
    virtual void edit_() = 0;

public slots:
    void edit() { edit_(); }
    void help() { QDesktopServices::openUrl(url_manual); }
};


template <class K> class TemplateEditObjectDialog : public EditObjectDialog
{
protected:
    K* database;

public:
    TemplateEditObjectDialog(const EnumIodeType iodeType, const QString& name, K* database, 
        QWidget* parent = Q_NULLPTR) : EditObjectDialog(iodeType, name, parent), database(database) {}
    ~TemplateEditObjectDialog() {}
};


class EditCommentDialog: public TemplateEditObjectDialog<KDBComments>
{
    Q_OBJECT

public:
    EditCommentDialog(const QString& name, KDBComments* database, QWidget* parent = Q_NULLPTR) : 
        TemplateEditObjectDialog(I_COMMENTS, name, database, parent) 
    {
        value = database->get(name.toStdString());
        textEdit_content->setPlainText(QString::fromStdString(value));
    }

private:
    void edit_() override
    {
        try
        {
            std::string name_ = lineEdit_name->text().toStdString();
            std::string value_ = textEdit_content->toPlainText().toStdString();
            
            if(name_ == name)
            {
                if(value_ == value)
                {
                    this->accept();
                    return;
                }

                database->update(name_, value_);
            }
            // check in / update the global IODE database since the variable 'database' may represents only a subset
            else if(Comments.contains(name_))
            {
                int res = QMessageBox::question(nullptr, "WARNING", QString::fromStdString(name_) + " already exists. Replace it ?");
                if(res != QMessageBox::StandardButton::Yes)
                {
                    this->reject();
                    return;
                }

                Comments.update(name_, value_);
            }
            else
                database->add(name_, value_);

            emit databaseModified();
            this->accept();
        }
        catch (const std::exception& e)
        {
            QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
        }
    }
};

class EditIdentityDialog: public TemplateEditObjectDialog<KDBIdentities>
{
    Q_OBJECT

public:
    EditIdentityDialog(const QString& name, KDBIdentities* database, QWidget* parent = Q_NULLPTR) : 
        TemplateEditObjectDialog(I_IDENTITIES, name, database, parent) 
    {
        value = database->get_lec(name.toStdString());
        textEdit_content->setPlainText(QString::fromStdString(value));
    }

private:
    void edit_() override
    {
        try
        {
            std::string name_ = lineEdit_name->text().toStdString();
            std::string value_ = textEdit_content->toPlainText().toStdString();
            
            if(name_ == name)
            {
                if(value_ == value)
                {
                    this->accept();
                    return;
                }

                database->update(name_, value_);
            }
            // check in the global IODE database since the variable 'database' may represents only a subset
            else if(Identities.contains(name_))
            {
                int res = QMessageBox::question(nullptr, "WARNING", QString::fromStdString(name_) + " already exists. Replace it ?");
                if(res != QMessageBox::StandardButton::Yes)
                {
                    this->reject();
                    return;
                }

                Identities.update(name_, value_);
            }
            else
                database->add(name_, value_);

            emit databaseModified();
            this->accept();
        }
        catch (const std::exception& e)
        {
            QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
        }
    }
};

class EditListDialog: public TemplateEditObjectDialog<KDBLists>
{
    Q_OBJECT

public:
    EditListDialog(const QString& name, KDBLists* database, QWidget* parent = Q_NULLPTR) : 
        TemplateEditObjectDialog(I_LISTS, name, database, parent) 
    {
        value = database->get(name.toStdString());
        textEdit_content->setPlainText(QString::fromStdString(value));
    }

private:
    void edit_() override
    {
        try
        {
            std::string name_ = lineEdit_name->text().toStdString();
            std::string value_ = textEdit_content->toPlainText().toStdString();
            
            if(name_ == name)
            {
                if(value_ == value)
                {
                    this->accept();
                    return;
                }

                database->update(name_, value_);
            }
            // check in the global IODE database since the variable 'database' may represents only a subset
            else if(Lists.contains(name_))
            {
                int res = QMessageBox::question(nullptr, "WARNING", QString::fromStdString(name_) + " already exists. Replace it ?");
                if(res != QMessageBox::StandardButton::Yes)
                {
                    this->reject();
                    return;
                }

                Lists.update(name_, value_);
            }
            else
                database->add(name_, value_);

            emit databaseModified();
            this->accept();
        }
        catch (const std::exception& e)
        {
            QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
        }
    }
};
