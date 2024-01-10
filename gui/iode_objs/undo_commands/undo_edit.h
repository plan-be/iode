#pragma once

#include <QObject>
#include <QUndoCommand>
#include <QMessageBox>

#include "iode_objs/models/comments_model.h"
#include "iode_objs/models/equations_model.h"
#include "iode_objs/models/identities_model.h"
#include "iode_objs/models/lists_model.h"
#include "iode_objs/models/scalars_model.h"
#include "iode_objs/models/tables_model.h"
#include "iode_objs/models/variables_model.h"

#include "cpp_api/iode_cpp_api.h"


class UndoEditValue : public QUndoCommand
{
protected:
    IodeAbstractTableModel* model;
    QString obj_name;
    QVariant new_value;
    QVariant old_value;

private:
    // We need to search the position of the name in the vertical header since running an IODE report 
    // or an IODE command may have change the corresponding IODE database 
    int find_row(const bool is_undo)
    {
        for(int i = 0; i < model->rowCount(); i++)
            if(obj_name == model()->headerData(row, Qt::Vertical).toString())
                return i;
        
        QString command = is_undo ? "Undo" : "Redo";
        QMessageBox::warning(nullptr, "WARNING", command + " command:\nObject named '" + obj_name + "' not found");
        return -1;
    }

public:
    UndoEditValue(IodeAbstractTableModel* model, const QString& obj_name, const QVariant& new_value, const QVariant& old_value) : 
        QUndoCommand("Edit '" + obj_name + "'"), model(model), obj_name(obj_name), new_value(new_value), old_value(old_value) {}

    void undo() override
    { 
        int row = find_row(true);
        if(row < 0)
            return;
        
        QModelIndex index = model->index(row, 1);
        model->setData(index, old_value, Qt::EditRole);
    }

    void redo() override
    { 
        int row = find_row(false);
        if(row < 0)
            return;

        QModelIndex index = model->index(row, 1);
        model->setData(index, new_value, Qt::EditRole);
    }
};
