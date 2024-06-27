#pragma once

#include "edit_iode_obj.h"


class EditIdentityDialog: public TemplateEditObjectDialog<KDBIdentities>
{
    Q_OBJECT

public:
    EditIdentityDialog(const QString& name, KDBIdentities* database, QWidget* parent = Q_NULLPTR) : 
        TemplateEditObjectDialog(IDENTITIES, name, database, parent) 
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
