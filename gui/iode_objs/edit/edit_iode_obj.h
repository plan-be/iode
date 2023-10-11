#pragma once

#include <QWidget>
#include <QString>
#include <QShortcut>
#include <QPlainTextEdit>

#include <string>

#include "ui_edit_iode_obj.h"
#include "utils.h"
#include "text_edit/completer.h"


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
    QString objectName;
    EnumIodeType iodeType;

    size_t hashBefore;
	size_t hashAfter;

    IodeCompleter* completer;

signals:
	void databaseModified();

public:
    EditObjectDialog(const EnumIodeType iodeType, const QString& objectName, QWidget* parent = Q_NULLPTR);
    ~EditObjectDialog();

protected:
    void computeHash(std::string& value, const bool before=false)
    {
        std::hash<std::string> str_hash;
        if(before)
            hashBefore = str_hash(value);
        else
        {
            hashAfter = str_hash(value);
            if(hashAfter != hashBefore) 
                emit databaseModified();
        }
    }

public slots:
    virtual void edit() = 0;
    void help();
};


class EditCommentDialog: public EditObjectDialog
{
    Q_OBJECT

public:
    EditCommentDialog(const QString& objectName, QWidget* parent = Q_NULLPTR);

public slots:
    void edit();
};

class EditIdentityDialog: public EditObjectDialog
{
    Q_OBJECT

public:
    EditIdentityDialog(const QString& objectName, QWidget* parent = Q_NULLPTR);

public slots:
    void edit();
};

class EditListDialog: public EditObjectDialog
{
    Q_OBJECT

public:
    EditListDialog(const QString& objectName, QWidget* parent = Q_NULLPTR);

public slots:
    void edit();
};
