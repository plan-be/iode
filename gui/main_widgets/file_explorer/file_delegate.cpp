#include "file_delegate.h"


QWidget* FileDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    return new QLineEdit(parent);
}

void FileDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QString old_name = index.model()->data(index, Qt::EditRole).toString();
    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(old_name);
}

void FileDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    QString new_name = lineEdit->text();
    QString old_name = model->data(index, Qt::DisplayRole).toString();
    // build path to the renamed file or directory
    QiodeFileExplorerProxyModel* fileModel = static_cast<QiodeFileExplorerProxyModel*>(model);
    QDir parent_dir = fileModel->fileInfo(index).absoluteDir();
    QString new_path = parent_dir.filePath(new_name);
    QString old_path = parent_dir.filePath(old_name);

    SystemItem file_item(QFileInfo(old_path), false);
    bool success = file_item.rename(new_name);
    if(!success)
        return;

    model->setData(index, new_name, Qt::EditRole);
}

void FileDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem itemOption(option);
    // set file with modified content in blue
    if (modifiedIndexes->contains(index)) itemOption.palette.setColor(QPalette::Text, QColor(Qt::blue));
    // set text items that have been cut in gray
    if (cutIndexes->contains(index)) itemOption.palette.setColor(QPalette::Text, QColor(Qt::gray));
    QStyledItemDelegate::paint(painter, itemOption, index);
}
