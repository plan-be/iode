#pragma once

#include <QWidget>
#include <QString>
#include <QShortcut>

#include "ui_estimation_coefs.h"
#include "utils.h"
#include "wrapper_classes.h"
#include "iode_objs/models/scalars_model.h"


class EstimationCoefsDialog : public QDialog, public Ui::EstimationCoefsDialog
{
    Q_OBJECT

	QShortcut* fullScreenShortcut;

public:
    EstimationCoefsDialog(EditAndEstimateEquations* edit_est_eqs, QWidget* parent = Q_NULLPTR) :
        QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint)
    {
        setupUi(this);

        QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";
        tableView_coefs->setStyleSheet(stylesheet);

        // NOTE: make a copy of the estimation scalars database because the database 
        //       passed to the constructor of ScalarsModel is deleted in its destructor 
        KDBScalars* kdb_scl = edit_est_eqs->get_scalars()->subset("*");
        ScalarsModel* scalarsModel = new ScalarsModel(this, kdb_scl);
        tableView_coefs->setModel(scalarsModel);

        fullScreenShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_X), this);
        fullScreenShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
        
        connect(fullScreenShortcut, &QShortcut::activated, this, &EstimationCoefsDialog::showMaximized);
    }

    ~EstimationCoefsDialog()
    {
        delete fullScreenShortcut;
    }
};
