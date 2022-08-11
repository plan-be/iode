#pragma once

#include <QWidget>
#include <QString>

#include <string>

#include "ui_add_object.h"
#include "utils.h"
#include "wrapper_classes.h"


/* NOTE FOR THE DEVELOPERS:
 * This Dialog class MUST
 * 1. use the extractAndVerify() method to extract input data from fields,
 * 2. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class QIodeAddComment : public QDialog, public Ui::QIodeAddObject
{
    Q_OBJECT

    WrapperIodeNameEdit* lineName;
    WrapperQLineEdit* lineDefinition;

public:
    QIodeAddComment(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

public slots:
    void add();
    void help();
};
