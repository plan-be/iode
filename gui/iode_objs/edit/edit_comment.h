#pragma once

#include "edit_iode_obj.h"


class EditCommentDialog: public TemplateEditObjectDialog<KDBComments>
{
    Q_OBJECT

public:
    EditCommentDialog(const QString& name, KDBComments* database, QWidget* parent = Q_NULLPTR) : 
        TemplateEditObjectDialog(COMMENTS, name, database, parent) 
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
