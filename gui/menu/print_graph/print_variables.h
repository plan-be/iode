#pragma once

#include <QWidget>
#include <QString>
#include <QTableView>

#include "ui_print_variables.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "print_abstract.h"
#include "text_edit/completer.h"

#ifndef GSAMPLE_NUMERICAL_WIDGET_HEADER
#define _GSAMPLE_
#include "tabs/tab_numerical_values.h"
#undef _GSAMPLE_
#endif


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class MenuPrintVariables: public MenuPrintAbstract, public Ui::MenuPrintVariables
{
    Q_OBJECT

    WrapperQPlainTextEdit*   wVariablesNames;
    IodeCompleter*          completer;

public:
	MenuPrintVariables(QWidget* parent = nullptr);
	~MenuPrintVariables();

public slots:
	void display();
	void print();
	void setup();
    void help();
};
