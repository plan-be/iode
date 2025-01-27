#pragma once
#include "edit_table_delegate.h"


bool EditTableDelegate::is_editable(const QModelIndex& index) const
{
	const EditTableModel* table_model = static_cast<const EditTableModel*>(index.model());
	TableLineType line_type = table_model->get_line_type(index.row());
	return (line_type == TABLE_LINE_TITLE) || (line_type == TABLE_LINE_CELL);
}

QWidget* EditTableDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (is_editable(index))
	{
		QLineEdit* editor = new QLineEdit(parent);
		return editor;
	}
	else
	{
		return nullptr;
	}
}

void EditTableDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	if (is_editable(index))
	{
		QString value = index.model()->data(index, Qt::EditRole).toString();
		QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
		lineEdit->setText(value);
	}
}

void EditTableDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	if (is_editable(index))
	{
		QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
		QString value = lineEdit->text();
		model->setData(index, value, Qt::EditRole);
	}
}
