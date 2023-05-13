#pragma once

#include <QWidget>

#include "ui_compute_model.h"
#include "utils.h"
#include "settings.h"
#include "wrapper_classes.h"
#include "custom_widgets/completer.h"


/* NOTE FOR THE DEVELOPERS:
 * All Menu Dialog classes MUST
 * 1. inherit from QIodeSettings,
 * 2. use the extractAndVerify() method to extract input data from fields,
 * 3. call the Q_OBJECT macro at the beginning of the class to allow slots and signals (see documentation of Qt).
 */

class QIodeMenuComputeModel : public QIodeSettings, public Ui::QIodeMenuComputeModel
{
    Q_OBJECT

    WrapperQPlainTextEdit* qEquationsList;
    QIodeCompleter*        completer;

public:
	QIodeMenuComputeModel(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QIodeMenuComputeModel();

public slots:
    void compute();
};
