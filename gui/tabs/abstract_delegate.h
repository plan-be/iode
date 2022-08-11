#pragma once

#include <QObject>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "../cpp_api/iode_cpp_api.h"
#include "utils.h"
#include "util/double_validator.h"


class AbstractDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	EnumIodeCase nameCase;

protected:
	QLineEdit* createNameEditor(QWidget* parent) const
	{
		QLineEdit* editor = new QLineEdit(parent);
		QString acceptedLetters;
		switch (nameCase)
		{
		case I_UPPER:
			acceptedLetters = "A-Z";
			break;
		case I_LOWER:
			acceptedLetters = "a-z";
			break;
		case I_ASIS:
			acceptedLetters = "A-Za-z";
			break;
		default:
			break;
		}
		// https://doc.qt.io/qt-5/qregexp.html
		QRegularExpression re(QString("[%1_][0-9%1_]{0,19}").arg(acceptedLetters));
		QValidator* validator = new QRegularExpressionValidator(re, editor);
		editor->setValidator(validator);
		return editor;
	}

	QLineEdit* createQLineEditor(QWidget* parent) const
	{
		QLineEdit* editor = new QLineEdit(parent);
		return editor;
	}

public:
	AbstractDelegate(EnumIodeCase nameCase, QObject* parent = nullptr) : QStyledItemDelegate(parent), nameCase(nameCase) {}

	~AbstractDelegate() {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const = 0;

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
