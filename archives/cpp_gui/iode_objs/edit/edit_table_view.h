#pragma once

#include <QWidget>
#include <QString>
#include <QTableView>
#include <QHeaderView>

#include <string>

#include "utils.h"
#include "wrapper_classes.h"
#include "edit_table_model.h"
#include "edit_table_delegate.h"


class EditTableView : public QTableView
{
    Q_OBJECT

public:
    EditTableView(QWidget* parent = Q_NULLPTR);

    void setupModel(const QString& name);

public slots:
    //void updateRows();
    void updateRow(const int row, const TableLineType line_type, const int nb_columns);
};
