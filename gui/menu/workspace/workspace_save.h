#pragma once

#include <QWidget>

#include "ui_workspace_save.h"
#include "../../utils.h"
#include "../../wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class QIodeMenuWorkspaceSave : public QIodeSettings, public Ui::QIodeMenuWorkspaceSave
{
	Q_OBJECT

public:
	QIodeMenuWorkspaceSave(QSettings& settings, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~QIodeMenuWorkspaceSave();

private:
    void save_component(KDBAbstract& kdb, const bool accept = true);

public slots:
    void save_comments();
    void save_equations();
    void save_identities();
    void save_lists();
    void save_scalars();
    void save_tables();
    void save_variables();
    void save();
};
