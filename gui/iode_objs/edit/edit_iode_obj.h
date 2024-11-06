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
    IodeTypes iodeType;

    IodeCompleter* completer;

signals:
	void databaseModified();

public:
    EditObjectDialog(const IodeTypes iodeType, const QString& name, QWidget* parent = Q_NULLPTR);
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
    TemplateEditObjectDialog(const IodeTypes iodeType, const QString& name, K* database, 
        QWidget* parent = Q_NULLPTR) : EditObjectDialog(iodeType, name, parent), database(database) {}
    ~TemplateEditObjectDialog() {}
};
