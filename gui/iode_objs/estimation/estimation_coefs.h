#pragma once

#include <QWidget>
#include <QString>

#include "ui_estimation_coefs.h"
#include "utils.h"
#include "wrapper_classes.h"
#include "iode_objs/models/scalars_model.h"


class QIodeEstimationCoefs : public QDialog, public Ui::QIodeEstimationCoefs
{
    Q_OBJECT

public:
    QIodeEstimationCoefs(KDBScalars* kdb_coefs, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) :
        QDialog(parent, f)
    {
        setupUi(this);

        QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";
        tableView_coefs->setStyleSheet(stylesheet);

        // Warning: need to create a copy of kdb_coefs because passed kdb is deleted in 
        //          the QIodeTemplateTableModel destructor
        ScalarsModel* scalarsModel = new ScalarsModel(this, new KDBScalars(*kdb_coefs));
        tableView_coefs->setModel(scalarsModel);
    }
};
