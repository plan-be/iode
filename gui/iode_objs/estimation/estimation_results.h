#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QVector>
#include <QGridLayout>

#include "ui_estimation_results.h"
#include "correlation_matrix_model.h"
#include "utils.h"
#include "wrapper_classes.h"
#include "iode_objs/models/scalars_model.h"


class QIodeEstimationResults : public QDialog, public Ui::QIodeEstimationResults
{
    Q_OBJECT

    int precision;
    Estimation* est;

private:
    void set_coefficients_tab();
    void set_correlation_matrix_tab();
    void set_tests_tab(Equation& eq);

public:
    QIodeEstimationResults(Estimation* est, QWidget* parent = Q_NULLPTR, 
        Qt::WindowFlags f = Qt::WindowFlags());

public slots:
    void plot_yobs_yest();
    void plot_residual();
    void print_graphs();
    void print_output();
    void help();
};
