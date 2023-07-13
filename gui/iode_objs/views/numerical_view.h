#pragma once

#include <QList>
#include <QMenu>
#include <QObject>
#include <QString>
#include <QShortcut>
#include <QClipboard>
#include <QMessageBox>

#include "abstract_table_view.h"
#include "iode_objs/delegates/base_delegate.h"
#include "iode_objs/models/abstract_table_model.h"


template <class M> class TemplateNumericalTableView : public TemplateTableView<M>
{
    QShortcut* shortcutCopy;
    QShortcut* shortcutCopyIncludingHeaders;
    QShortcut* shortcutPast;

protected:
    QMenu* contextMenu;

protected:
    QAction* addAction(const QString& name, const QString& tooltip, const QKeySequence& shortcut = QKeySequence())
    {
        QAction* action = new QAction(name, this);
        action->setToolTip(tooltip);
        action->setVisible(true);
        if(!shortcut.isEmpty()) action->setShortcut(shortcut);
        contextMenu->addAction(action);
        return action;
    }

    void contextMenuEvent(QContextMenuEvent* event) override
    {
        contextMenu->popup(event->globalPos());
        event->accept();
    }

private:
	void setupContextMenu()
    {
        QAction* action;
        contextMenu = new QMenu(this);

        action = addAction("Copy", "Copy selected values to the clipboard", shortcutCopy->key());
        QObject::connect(action, &QAction::triggered, this, &TemplateNumericalTableView::copy);

        action = addAction("Copy Including Headers", "Copy selected values and headers to the clipboard", shortcutCopyIncludingHeaders->key());
        QObject::connect(action, &QAction::triggered, this, &TemplateNumericalTableView::copyIncludingHeaders);

        action = addAction("Paste", "Paste values contained in the clipboard", shortcutPast->key());
        QObject::connect(action, &QAction::triggered, this, &TemplateNumericalTableView::paste);
    }

public:
    TemplateNumericalTableView(EnumIodeType iodeType, BaseDelegate* delegate, QWidget* parent = nullptr):
        TemplateTableView<M>(iodeType, delegate, parent) 
    {
        shortcutCopy = new QShortcut(QKeySequence::Copy, this);
        shortcutCopyIncludingHeaders = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C), this);
        shortcutPast = new QShortcut(QKeySequence::Paste, this);

        shortcutCopy->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutCopyIncludingHeaders->setContext(Qt::WidgetWithChildrenShortcut);
        shortcutPast->setContext(Qt::WidgetWithChildrenShortcut);

        QObject::connect(shortcutCopy, &QShortcut::activated, this, &TemplateNumericalTableView::copy);
        QObject::connect(shortcutCopyIncludingHeaders, &QShortcut::activated, this, &TemplateNumericalTableView::copyIncludingHeaders);
        QObject::connect(shortcutPast, &QShortcut::activated, this, &TemplateNumericalTableView::paste);

        // context menu = popup menu displayed when a user right clicks
        setupContextMenu();
    }

    ~TemplateNumericalTableView()
    {
        delete contextMenu;

        delete shortcutCopy;
        delete shortcutCopyIncludingHeaders;
        delete shortcutPast;
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
        try
        {
            QModelIndex start = this->selectionModel()->selectedIndexes().first();

            QClipboard* clipboard = QApplication::clipboard();
            QString text = clipboard->text();

            M* model_ = static_cast<M*>(AbstractTableView::model());

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

protected:
    QStringList extractObjectsNames()
    {
        QItemSelectionRange selection_range =  this->selectionModel()->selection().first();

        M* model_ = static_cast<M*>(AbstractTableView::model());

        QStringList objectNames;
        for(int row = selection_range.top(); row <= selection_range.bottom(); row++)
            objectNames << model_->headerData(row, Qt::Vertical, Qt::DisplayRole).toString();

        return objectNames;
    }

    QStringList extractSample()
    {
        QItemSelectionRange selection_range =  this->selectionModel()->selection().first();

        M* model_ = static_cast<M*>(AbstractTableView::model());

        QStringList sample;
        for(int column = selection_range.left(); column <= selection_range.right(); column++)
            sample << model_->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();

        return sample;
    }

	QList<QStringList> extractValues()
    {

        QItemSelectionRange selection_range =  this->selectionModel()->selection().first();

        M* model_ = static_cast<M*>(AbstractTableView::model());

        QString value;
        QList<QStringList> values;
        for(int row = selection_range.top(); row <= selection_range.bottom(); row++)
        {
            QStringList rowValues;
            for(int column = selection_range.left(); column <= selection_range.right(); column++)
            {
                value = model_->dataCell(row, column).toString();
                rowValues << ((value == NAN_REP) ? "" : value);
            }
            values.push_back(rowValues);
        }
        
        return values;
    }
};
