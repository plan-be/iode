#pragma once

#include <QWidget>
#include <QString>

#include <string>
#include <array>

#include "ui_dyn_adjustment.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * This Dialog class MUST
 * 1. use the extractAndVerify() method to extract input data from fields,
 * 2. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class QIodeDynAdjustment : public QIodeSettings, public Ui::QIodeDynAdjustment
{
    Q_OBJECT

    QString& lec;

    WrapperComboBox* comboMethod;
    WrapperQLineEdit* lineCoef1;
    WrapperQLineEdit* lineCoef2;

public:
    QIodeDynAdjustment(QString& lec, const QString& settings_filepath, QWidget* parent = Q_NULLPTR, 
        Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeDynAdjustment();

public slots:
    void adjust();
    void help();
};