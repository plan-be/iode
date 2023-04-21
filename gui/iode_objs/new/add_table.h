#pragma once

#include <QWidget>
#include <QString>

#include <string>

#include "ui_add_table.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "custom_widgets/qiode_completer.h"


/* NOTE FOR THE DEVELOPERS:
 * This Dialog class MUST
 * 1. use the extractAndVerify() method to extract input data from fields,
 * 2. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class QIodeAddTable : public QIodeSettings, public Ui::QIodeAddTable
{
    Q_OBJECT

    WrapperIodeNameEdit*   lineName;
    WrapperSpinBox*        spinNbColumns;
    WrapperQLineEdit*      lineTitle;
    WrapperQPlainTextEdit* textLEC;
    WrapperCheckBox*       checkBoxMode;
    WrapperCheckBox*       checkBoxFiles;
    WrapperCheckBox*       checkBoxDate;

    QIodeCompleter* completer;

public:
    QIodeAddTable(const QString& project_settings_filepath, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeAddTable();

public slots:
    void add();
    void help();
};