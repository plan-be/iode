#pragma once

#include <QWidget>
#include <QString>

#include <string>
#include <array>

#include "ui_unit_root.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * This Dialog class MUST
 * 1. use the extractAndVerify() method to extract input data from fields,
 * 2. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */


class QIodeUnitRoot : public QIodeSettings, public Ui::QIodeUnitRoot
{
    Q_OBJECT

    int precision;

    WrapperQLineEdit* lineSeries;
    WrapperSpinBox* spinBoxOrder;

private:
    QString get_tstat(Scalar& scalar);

public:
    QIodeUnitRoot(QWidget* parent = Q_NULLPTR, 
        Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeUnitRoot();

public slots:
    void analyse();
    void help();
};