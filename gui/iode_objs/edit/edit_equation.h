#pragma once

#include <QWidget>
#include <QMap>
#include <QString>

#include <string>
#include <array>
#include <stdexcept>

#include "ui_edit_equation.h"
#include "edit_vars_sample.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "main_window_abstract.h"
#include "iode_objs/views/scalars_view.h"
#include "iode_objs/estimation/estimation_coefs.h"
#include "iode_objs/estimation/estimation_results.h"
#include "iode_objs/misc/unit_root.h"
#include "iode_objs/misc/dyn_adjustment.h"
#include "text_edit/completer.h"


/* NOTE FOR THE DEVELOPERS:
 * This Dialog class MUST
 * 1. use the extractAndVerify() method to extract input data from fields,
 * 2. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class EditEquationDialog : public IodeSettingsDialog, public Ui::EditEquationDialog
{
    Q_OBJECT

    EditAndEstimateEquations edit_est_eqs;	

    IodeCompleter* completer;

    KDBEquations* database;
    Equation* eq;

    WrapperIodeNameEdit*    lineName;
    WrapperComboBox*        comboBoxMethod;
    WrapperSampleEdit*      sampleFrom;
    WrapperSampleEdit*      sampleTo;
    WrapperQPlainTextEdit*  lineLec;
    WrapperQLineEdit*       lineComment;
    WrapperQLineEdit*       lineBlock;
    WrapperQLineEdit*       lineInstruments;

	QShortcut* fullScreenShortcut;

protected:
	QString project_settings_filepath;

private:
    void save_current_equation();
    void update_list_equations_to_estimate();
    void display_equation();

public:
    EditEquationDialog(const QString& equationName, KDBEquations* database, QWidget* parent = Q_NULLPTR);
    ~EditEquationDialog();

signals:
    void databaseModified();
    void newObjectInserted(QString);

public slots:
    void edit();
    void display_coefs();
    void estimate();
    void next();
    void dynamic_adjustment();
    void results();
    void unit_root();
    void help();
};
