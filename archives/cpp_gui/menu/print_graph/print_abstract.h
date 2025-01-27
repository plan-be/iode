#pragma once

#include <QWidget>
#include <QString>
#include <QPrinter>
#include <QStringList>
#include <QTextDocument>
#include <QPrintPreviewDialog>

#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "print/print.h"
#include "menu/file/file_settings.h"
#include "print/print_file_dialog.h"

/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from IodeSettingsDialog,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class MenuPrintAbstract: public IodeSettingsDialog
{
    Q_OBJECT 

    QPrinter printer;
    QTextDocument document;

protected:
    WrapperQTextEdit*        wSample;
    WrapperFileChooser*      wFile2;
    WrapperFileChooser*      wFile3;
    WrapperFileChooser*      wFile4;
    WrapperFileChooser*      wFile5;
    WrapperComboBox*         wLanguage;
    WrapperSpinBox*          wNbDecimals;

public:
    MenuPrintAbstract(QWidget* parent = nullptr) 
        : IodeSettingsDialog(parent) {}

    ~MenuPrintAbstract() {}

protected:
    void printTableOrVariable(const bool isTable, const QString& names);

private slots:
    void renderForPrinting()
    {
        document.print(&printer);
    }
};
