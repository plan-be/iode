#pragma once

#include <QWidget>

#include "ui_workspace_save.h"
#include "../../utils.h"
#include "../../settings.h"
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

    bool save_all;
    int nb_comments;
    int nb_equations;
    int nb_identities;
    int nb_lists;
    int nb_scalars;
    int nb_tables;
    int nb_variables;

public:
	QIodeMenuWorkspaceSave(const QString& settings_filepath, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~QIodeMenuWorkspaceSave();

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
