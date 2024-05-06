#pragma once

#include <QObject>
#include <QDialog>
#include <QString>
#include <QWidget>
#include <QVBoxLayout>

#include "tab_numerical_values.h"
#include "iode_objs/models/computed_table_model.h"
#include "iode_objs/views/computed_table_view.h"

/* NOTE FOR THE DEVELOPERS:
 * Multiple Inheritance Requires QObject to Be First
 * If you are using multiple inheritance, moc assumes that the first inherited class
 * is a subclass of QObject. Also, be sure that only the first inherited class is a QObject.
 * https://doc.qt.io/qt-6/moc.html#multiple-inheritance-requires-qobject-to-be-first
 * --> That's why the class below does not inherit from NumericalTableView but contains 
 *     a member of type NumericalTableView
 */


class ComputedTableNumericalDialog : public QDialog
{
    Q_OBJECT

    NumericalWidget numeric; 

    QVBoxLayout* vLayout;

    ComputedTableView*  tableview;
    ComputedTableModel* objmodel;

protected:
    QString getGroupName()
    {
        return "GSAMPLE_TABLE";
    }

    void closeEvent(QCloseEvent* event) override
    {
        saveSettings();
        event->accept();
    }

public:
    ComputedTableNumericalDialog(const QString& refTable, const QString& gsample, const int nbDecimals, 
        const QString& variables, QWidget *parent = nullptr): 
        QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint) 
    {
        setGeometry(QRect(10, 50, 700, 250));

        vLayout = new QVBoxLayout(this);
        vLayout->setContentsMargins(0, 0, 0, 0);
        vLayout->setObjectName("vTabLayout");

        tableview = new ComputedTableView(this);
        objmodel = new ComputedTableModel(refTable, gsample, nbDecimals, variables, tableview);
        tableview->setModel(objmodel);
        tableview->horizontalHeader()->setStretchLastSection(true);

        QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";
        tableview->setStyleSheet(stylesheet);

        // -1 -> span over all rows/columns
        vLayout->addWidget(tableview);

        QHBoxLayout* bottomLayout = numeric.setup(this, objmodel, tableview);

        QSpacerItem* horizontalSpacer = new QSpacerItem(800, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        // -1 -> span over all rows/columns
        bottomLayout->addSpacerItem(horizontalSpacer);

        vLayout->addLayout(bottomLayout);

        loadSettings();

        this->setModal(true);
        if(variables.isEmpty())
            this->setWindowTitle("Table " + refTable);
        else
            this->setWindowTitle("Table of series");
    }

    ~ComputedTableNumericalDialog()
    {
        delete objmodel;
        delete tableview;
    }

public slots:
    void loadSettings()
    {
        QSettings* project_settings = ProjectSettings::getProjectSettings();
        if(!project_settings)
            return;

        project_settings->beginGroup(getGroupName());
        numeric.loadNumericSettings(project_settings);
        project_settings->endGroup();
    }

    void saveSettings()
    {
        QSettings* project_settings = ProjectSettings::getProjectSettings();
        if(!project_settings)
            return;

        project_settings->beginGroup(getGroupName());
        numeric.saveNumericSettings(project_settings);
        project_settings->endGroup();
    }
};
