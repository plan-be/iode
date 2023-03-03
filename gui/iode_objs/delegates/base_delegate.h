#pragma once

#include <QObject>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "cpp_api/iode_cpp_api.h"
#include "utils.h"
#include "util/double_validator.h"


class BaseDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	EnumIodeCase nameCase;

public:
	BaseDelegate(EnumIodeCase nameCase, QObject* parent = nullptr) : QStyledItemDelegate(parent), nameCase(nameCase) {}

	~BaseDelegate() {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QLineEdit* editor = new QLineEdit(parent);
		return editor;
	}

	void setEditorData(QWidget* editor, const QModelIndex& index) const override
	{
		QString value = index.model()->data(index, Qt::EditRole).toString();
		QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
		lineEdit->setText(value);
	}

	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
	{
		QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
		QString value = lineEdit->text();
		model->setData(index, value, Qt::EditRole);
	}
};
