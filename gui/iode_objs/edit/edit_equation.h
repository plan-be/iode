#pragma once

#include <QWidget>
#include <QMap>
#include <QString>

#include <string>
#include <array>

#include "ui_edit_equation.h"
#include "edit_vars_sample.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * This Dialog class MUST
 * 1. use the extractAndVerify() method to extract input data from fields,
 * 2. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class QIodeEditEquation : public QIodeSettings, public Ui::QIodeEditEquation
{
    Q_OBJECT

    KDBEquations kdb;

    WrapperIodeNameEdit* lineName;
    WrapperComboBox* comboBoxMethod;
    WrapperSampleEdit* sampleFrom;
    WrapperSampleEdit* sampleTo;
    WrapperQTextEdit* lineLec;
    WrapperQLineEdit* lineComment;
    WrapperQLineEdit* lineBlock;
    WrapperQLineEdit* lineInstruments;

public:
    QIodeEditEquation(const QString& equationName, const QString& project_settings_filepath, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeEditEquation();

public slots:
    void edit();
    void help();
};
