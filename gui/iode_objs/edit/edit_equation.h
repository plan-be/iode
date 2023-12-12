#pragma once

#include <QWidget>
#include <QMap>
#include <QString>
#include <QList>

#include <string>
#include <array>

#include "ui_edit_equation.h"
#include "edit_vars_sample.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
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

    EstimationResults*  est_results;
    QStringList         eqs_list;
    QString             next_block;
    
    size_t hashBefore;
	size_t hashAfter;	

    IodeCompleter* completer;

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
    void display_equation(const QString& equationName, const bool update_block_from_equation=false);

	void computeHash(Equation& eq, const bool before=false)
	{
		boost::hash<Equation> eq_hasher;
		if(before)
    		hashBefore = eq_hasher(eq);
		else
		{
			hashAfter = eq_hasher(eq);
			if(hashAfter != hashBefore) 
				emit databaseModified();
		}
	}

public:
    EditEquationDialog(const QString& equationName, QWidget* parent = Q_NULLPTR);
    ~EditEquationDialog();

signals:
    void databaseModified();

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
