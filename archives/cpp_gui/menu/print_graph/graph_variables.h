#pragma once

#include <QWidget>

#include "ui_graph_variables.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "plot/plot_vars.h"
#include "text_edit/completer.h"
#include "menu/file/file_settings.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from IodeSettingsDialog,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class MenuGraphVariables: public IodeSettingsDialog, public Ui::MenuGraphVariables
{
    Q_OBJECT

    WrapperQPlainTextEdit*  wVariables;
    WrapperComboBox*        wVarMode;
    WrapperSampleEdit*      wFrom;
    WrapperSampleEdit*      wTo;
    WrapperComboBox*        wChartType;
    WrapperCheckBox*        wYAxisScale;
    WrapperComboBox*        wXTicks;
    WrapperComboBox*        wYTicks;
    WrapperQLineEdit*       wMinY;
    WrapperQLineEdit*       wMaxY;
    WrapperComboBox*        wLanguage;

    IodeCompleter*         completer;

public:
	MenuGraphVariables(QWidget* parent = nullptr);
	~MenuGraphVariables();

    void setVariablesNames(const QList<QString>& VariablesNames)
    {
        QString inputString;
        foreach(const QString& name, VariablesNames) inputString += name + ";";
        wVariables->setQValue(inputString);
    }

    void setFrom(const QString& from)
    {
        wFrom->setQValue(from);
    }

    void setTo(const QString& to)
    {
        wTo->setQValue(to);
    }

signals:
    void newPlot(QDialog* plotDialog);

public slots:
	void display();
	void apply();
	void setup();
    void help();
};
