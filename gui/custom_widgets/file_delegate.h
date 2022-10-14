#pragma once 

#include <QObject>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QMessageBox>
#include <QModelIndexList>

#include "util/system_item.h"
#include "qiode_file_explorer_proxy.h"


class FileDelegate : public QStyledItemDelegate
{
    QModelIndexList* cutIndexes;

public:
    FileDelegate(QModelIndexList* cutIndexes, QObject *parent = nullptr) : QStyledItemDelegate(parent), cutIndexes(cutIndexes) {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index);

	void setEditorData(QWidget* editor, const QModelIndex& index) const override;

	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
