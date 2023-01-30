#pragma once

#include <QDialog>
#include <QTableView>
#include <QHeaderView>

#include "iode_objs/models/gsample_table_model.h"


class QIodeGSampleTableView : public QDialog
{
    Q_OBJECT

    QTableView* tableview;
    GSampleTableModel* model;

public:
    QIodeGSampleTableView(const QString& refTable, const QString& gsample, const int nbDecimals, 
        const QString& variables, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) : QDialog(parent, f)
    {
        QTableView* tableview = new QTableView(this);
        GSampleTableModel* model = new GSampleTableModel(refTable, gsample, nbDecimals, variables);
        tableview->setModel(model);

        tableview->setGeometry(QRect(10, 50, 700, 250));

        QSizePolicy sizePolicyTable(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicyTable.setHorizontalStretch(0);
        sizePolicyTable.setVerticalStretch(0);
        sizePolicyTable.setHeightForWidth(tableview->sizePolicy().hasHeightForWidth());
        tableview->setSizePolicy(sizePolicyTable);
        tableview->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
        tableview->horizontalHeader()->setStretchLastSection(true);

        QString stylesheet = "QHeaderView::section { background-color: lightGray; font: bold; border: 0.5px solid }";
        tableview->setStyleSheet(stylesheet);

        this->setModal(true);
        if(variables.isEmpty())
            this->setWindowTitle(refTable);
        else
            this->setWindowTitle("Table of variables");
    }

    // Note: the members model and tableview are already deleted when entering the destructor
    ~QIodeGSampleTableView() { }
};
