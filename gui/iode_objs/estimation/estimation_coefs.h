#pragma once

#include <QWidget>
#include <QString>

#include "ui_estimation_coefs.h"
#include "utils.h"
#include "wrapper_classes.h"
#include "iode_objs/models/scalars_model.h"


class EstimationCoefsDialog : public QDialog, public Ui::EstimationCoefsDialog
{
    Q_OBJECT

	QShortcut* fullScreenShortcut;

public:
    EstimationCoefsDialog(KDBScalars* kdb_coefs, QWidget* parent = Q_NULLPTR) :
        QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint)
    {
        setupUi(this);

        QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";
        tableView_coefs->setStyleSheet(stylesheet);

        ScalarsModel* scalarsModel = new ScalarsModel(this, kdb_coefs);
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
