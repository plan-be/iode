#pragma once

#include <QDialog>
#include <QTableView>
#include <QHeaderView>

#include "iode_objs/models/gsample_table_model.h"


class QIodeGSampleTableView : public QDialog
{
    Q_OBJECT

public:
    QIodeGSampleTableView(const QString& refTable, const QString& gsample, const int nbDecimals, 
        const QString& variables, QWidget* parent = Q_NULLPTR) : QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint)
    {
        this->setGeometry(QRect(10, 50, 700, 250));

        QGridLayout* gridLayout = new QGridLayout(this);
        gridLayout->setObjectName("gridLayoutGSampleTable");

        QTableView* tableview = new QTableView(this);
        GSampleTableModel* model = new GSampleTableModel(refTable, gsample, nbDecimals, variables, tableview);
        tableview->setModel(model);
        tableview->horizontalHeader()->setStretchLastSection(true);

        QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";
        tableview->setStyleSheet(stylesheet);

        gridLayout->addWidget(tableview, 0, 0, 1, 1);

        this->setModal(true);
        if(variables.isEmpty())
            this->setWindowTitle(refTable);
        else
            this->setWindowTitle("Table of series");
    }

    // Note: the members model and tableview are already deleted when entering the destructor
    ~QIodeGSampleTableView() { }
};
