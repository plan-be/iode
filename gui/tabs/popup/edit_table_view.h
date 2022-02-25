#pragma once

#include <QWidget>
#include <QString>
#include <QTableView>
#include <QHeaderView>

#include <string>

#include "../../utils.h"
#include "../../wrapper_classes.h"
#include "edit_table_model.h"
#include "edit_table_delegate.h"


class QIodeEditTableView : public QTableView
{
    Q_OBJECT

public:
    QIodeEditTableView(QWidget* parent = Q_NULLPTR);

    void setupModel(const QString& tableName);

public slots:
    //void updateRows();
    void updateRow(const int row, const EnumLineType line_type, const int nb_columns);
};
