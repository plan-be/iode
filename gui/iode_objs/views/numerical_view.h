#pragma once

#include <QList>
#include <QMenu>
#include <QObject>
#include <QString>
#include <QShortcut>
#include <QClipboard>
#include <QTableView>
#include <QMessageBox>

#include "utils.h"
#include "abstract_table_view.h"
#include "iode_objs/models/numerical_table_model.h"
#include "iode_objs/delegates/base_delegate.h"
#include "iode_objs/models/abstract_table_model.h"


class NumericalTableView: public QObject
{
    Q_OBJECT

    bool         allowToPaste;
    QTableView*  owner;

    QShortcut*   shortcutCopy;
    QShortcut*   shortcutCopyIncludingHeaders;
    QShortcut*   shortcutPast;
    QShortcut*   shortcutPastVars;

protected:
    QMenu* contextMenu;

private:
	void setupContextMenu_()
    {
        QAction* action;
        contextMenu = new QMenu(owner);

        action = addAction_("Copy", "Copy selected values to the clipboard", shortcutCopy->key());
        QObject::connect(action, &QAction::triggered, this, &NumericalTableView::copy);

        action = addAction_("Copy Including Headers", "Copy selected values and headers to the clipboard", shortcutCopyIncludingHeaders->key());
        QObject::connect(action, &QAction::triggered, this, &NumericalTableView::copyIncludingHeaders);

        if(allowToPaste)
        {            
            action = addAction_("Paste", "Paste values contained in the clipboard", shortcutPast->key());
            QObject::connect(action, &QAction::triggered, this, &NumericalTableView::paste);

            action = addAction_("Paste Special", "Paste values according to variable names and optionally periods", shortcutPastVars->key());
            QObject::connect(action, &QAction::triggered, this, &NumericalTableView::paste_vars);        
        }
    }

public:
    NumericalTableView(const bool allowToPaste): allowToPaste(allowToPaste), owner(nullptr) {}

    void setup(QTableView* owner)
    {
        this->owner = owner; 

        shortcutCopy = new QShortcut(QKeySequence::Copy, owner);
        shortcutCopy->setContext(Qt::WidgetWithChildrenShortcut);
        QObject::connect(shortcutCopy, &QShortcut::activated, this, &NumericalTableView::copy);
        
        shortcutCopyIncludingHeaders = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C), owner);
        shortcutCopyIncludingHeaders->setContext(Qt::WidgetWithChildrenShortcut);
        QObject::connect(shortcutCopyIncludingHeaders, &QShortcut::activated, this, &NumericalTableView::copyIncludingHeaders);
        
        if(allowToPaste)
        {
            shortcutPast = new QShortcut(QKeySequence::Paste, owner);
            shortcutPast->setContext(Qt::WidgetWithChildrenShortcut);
            QObject::connect(shortcutPast, &QShortcut::activated, this, &NumericalTableView::paste);

            shortcutPastVars = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_V), owner);
            shortcutPastVars->setContext(Qt::WidgetWithChildrenShortcut);
            QObject::connect(shortcutPastVars, &QShortcut::activated, this, &NumericalTableView::paste_vars);
        }

        // headers
        owner->horizontalHeader()->setStretchLastSection(false);
        owner->verticalHeader()->setStretchLastSection(false);

        // context menu = popup menu displayed when a user right clicks
        setupContextMenu_();
    }

    ~NumericalTableView()
    {
        delete contextMenu;

        delete shortcutCopy;
        delete shortcutCopyIncludingHeaders;
        if(allowToPaste)
        {
            delete shortcutPast;
            delete shortcutPastVars;
        }
    }

    QAction* addAction_(const QString& name, const QString& tooltip, const QKeySequence& shortcut = QKeySequence())
    {
        QAction* action = new QAction(name, owner);
        action->setToolTip(tooltip);
        action->setVisible(true);
        if(!shortcut.isEmpty()) action->setShortcut(shortcut);
        contextMenu->addAction(action);
        return action;
    }

    /**
     * @warning All classes using NumericalTableView must override contextMenuEvent()
     *          and call popupContextMenu() inside contextMenuEvent()
     */
    void popupContextMenu(QContextMenuEvent* event)
    {
        contextMenu->popup(event->globalPos());
    }

    void copy()
    {
        try
        {
            QList<QStringList> values = extractValues();

            QString text;
            foreach(const QStringList& rowValues, values)
                text += rowValues.join("\t") + "\n";

            QClipboard* clipboard = QApplication::clipboard();
            clipboard->setText(text);
        }
        catch(const std::exception& e)
        {
            QMessageBox::warning(nullptr, "WARNING", "Can't copy values\n\n" + QString::fromStdString(e.what()));
        }
    }

    void copyIncludingHeaders()
    {
        try
        {
            QStringList objectsNames = extractObjectsNames();
            QStringList sample = extractSample();
            QList<QStringList> values = extractValues();

            QString text = "\t" + sample.join("\t") + "\n";
            for(int row = 0; row < values.size(); row++)
                text += objectsNames[row] + "\t" + values.at(row).join("\t") + "\n";

            QClipboard* clipboard = QApplication::clipboard();
            clipboard->setText(text);
        }
        catch(const std::exception& e)
        {
            QMessageBox::warning(nullptr, "WARNING", "Can't copy values\n\n" + QString::fromStdString(e.what()));
        }
    }

    void paste()
    {
        if(allowToPaste)
        {
            try
            {
                QModelIndex start = owner->selectionModel()->selectedIndexes().first();
                QAbstractItemModel* model_ = owner->model();

                QClipboard* clipboard = QApplication::clipboard();
                QString text = clipboard->text();

                // If the text from clipboard ends with the new-line character (which is the case when we copy 
                // from Excel), the QString::split() function returns a list ending by an empty element. 
                // This empty element is then interpreted as a new NaN value by the setData function.
                if(text.endsWith("\n"))
                    text.chop(1);

                int i=0, j=0;
                foreach(const QString& row_string, text.split("\n"))
                {
                    j=0;
                    foreach(const QString& value, row_string.split("\t"))
                    {
                        QModelIndex index = model_->index(start.row() + i , start.column()+ j);
                        if(index.isValid())
                            model_->setData(index, value, Qt::EditRole);
                        j++;
                    }
                    i++;
                }
            }
            catch(const std::exception& e)
            {
                QMessageBox::warning(nullptr, "WARNING", "Can't paste values\n\n" + QString::fromStdString(e.what()));
            }
        }
    }

    void paste_vars()
    {
        if(allowToPaste)
        {
            try
            {
                QModelIndex start = owner->selectionModel()->selectedIndexes().first();
                QAbstractItemModel* model_ = owner->model();

                QClipboard* clipboard = QApplication::clipboard();
                QString text = clipboard->text();

                // If the text from clipboard ends with the new-line character (which is the case when we copy 
                // from Excel), the QString::split() function returns a list ending by an empty element. 
                // This empty element is then interpreted as a new NaN value by the setData function.
                if(text.endsWith("\n"))
                    text.chop(1);

                QList<QString> lines = text.split("\n");
                if(lines.count() == 0)
                    return;

                // ---- extract columns ----
                int start_column;
                QString from;
                QString to;
                Sample* sample_vars = Variables.get_sample();
                
                // check if the first line represents the periods 
                QList<QString> cells = lines[0].split("\t");
                if(cells[0].isEmpty())
                {
                    from = cells[1];
                    to = cells.last();
                    lines.removeFirst();
                }
                else
                    ask_sample(from, to);
                
                // check that the passed sample has the size equal to the number of cells per line.
                // We compare to cells.count() -1 because the first cell is supposed to contains 
                // the variable name, not a value
                Sample sample(from.toStdString(), to.toStdString());
                if(sample.nb_periods() != cells.count() - 1)
                {
                    QMessageBox::warning(nullptr, "WARNING", QString("Can't paste values.\n") + 
                        "The passed sample " + QString::fromStdString(sample.to_string()) + 
                        " does not correspond to number of values per variable: '" + 
                        QString::number(cells.count()) + "'");
                    return;
                }
                
                start_column = sample_vars->get_period_position(from.toStdString());

                int row=0; 
                int column=0;
                QString var_name;
                foreach(const QString& line, lines)
                {
                    QList<QString> cells = line.split("\t");

                    var_name = cells.takeFirst().trimmed();
                    // raises an error if the the variable name is not found
                    row = Variables.get_position(var_name.toStdString());

                    column = start_column;
                    foreach(const QString& value, cells)
                    {
                        QModelIndex index = model_->index(row, column);
                        if(index.isValid())
                            model_->setData(index, value, Qt::EditRole);
                        column++;
                    }
                }
            }
            catch(const std::exception& e)
            {
                QMessageBox::warning(nullptr, "WARNING", "Can't paste values\n\n" + QString::fromStdString(e.what()));
            }
        }
    }

    QStringList extractObjectsNames()
    {
        QItemSelectionRange selection_range =  owner->selectionModel()->selection().first();
        QAbstractItemModel* model_ = owner->model();

        QStringList objectNames;
        for(int row = selection_range.top(); row <= selection_range.bottom(); row++)
            objectNames << model_->headerData(row, Qt::Vertical, Qt::DisplayRole).toString();

        return objectNames;
    }

    QStringList extractSample()
    {
        QItemSelectionRange selection_range =  owner->selectionModel()->selection().first();
        QAbstractItemModel* model_ = owner->model();

        QStringList sample;
        for(int column = selection_range.left(); column <= selection_range.right(); column++)
            sample << model_->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();

        return sample;
    }

	QList<QStringList> extractValues()
    {
        no_precision = true;
        QItemSelectionRange selection_range =  owner->selectionModel()->selection().first();
        QAbstractItemModel* model_ = owner->model();

        QString value;
        QList<QStringList> values;
        for(int row = selection_range.top(); row <= selection_range.bottom(); row++)
        {
            QStringList rowValues;
            for(int column = selection_range.left(); column <= selection_range.right(); column++)
            {
                value = model_->data(model_->index(row, column), Qt::DisplayRole).toString();
                rowValues << ((value == NAN_REP) ? "" : value);
            }
            values.push_back(rowValues);
        }

        no_precision = false;
        return values;
    }

    void ask_sample(QString& from, QString& to)
    {
        try
        {
            EditIodeSampleDialog dialog(owner);
            if (dialog.exec() == QDialog::Accepted)
            {
                from = dialog.get_from();
                to = dialog.get_to();
            }
        }
        catch(const std::exception& e)
        {
            QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
        }
    }
};
