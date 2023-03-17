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
    // build path to the renamed file or directory
    QiodeFileExplorerProxyModel* fileModel = static_cast<QiodeFileExplorerProxyModel*>(model);
    QDir parent_dir = fileModel->fileInfo(index).absoluteDir();
    QString new_path = parent_dir.filePath(new_name);
    QFileInfo fileInfo(new_path);
    // user tries to rename a file or a directory with the same name as an existing one
    if (fileInfo.exists())
    {
        QString type = fileInfo.isDir() ? "Directory" : "File";
        QMessageBox::StandardButton answer = QMessageBox::warning(nullptr, "Warning", type + " " + 
            fileInfo.fileName() + " already exist!\n Overwrite it ?", 
            QMessageBox::Yes | QMessageBox::Discard, QMessageBox::Yes);
        if (answer == QMessageBox::Discard) return;
        // delete existing file or directory
        SystemItem(fileModel->fileInfo(index)).remove();
    }
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
