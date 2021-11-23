#pragma once

#include <QObject>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QRegularExpression>
#include <QRegularExpressionValidator>


class AbstractDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	bool uppercase;

protected:
	QLineEdit* createNameEditor(QWidget* parent) const
	{
		QLineEdit* editor = new QLineEdit(parent);
		QString acceptedLetters = uppercase ? "A-Z" : "a-z";
		// https://doc.qt.io/qt-5/qregexp.html
		QRegularExpression re(QString("[%1_][0-9%1_]{0,19}").arg(acceptedLetters));
		QValidator* validator = new QRegularExpressionValidator(re, editor);
		editor->setValidator(validator);
		return editor;
	}

	QLineEdit* createTextEditor(QWidget* parent) const
	{
		QLineEdit* editor = new QLineEdit(parent);
		return editor;
	}

public:
	AbstractDelegate(bool uppercase, QObject* parent = nullptr) : QStyledItemDelegate(parent), uppercase(uppercase) {}

	~AbstractDelegate() {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const = 0;

	void setEditorData(QWidget* editor, const QModelIndex& index) const override
	{
		QString value = index.model()->data(index, Qt::EditRole).toString();
		QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
		lineEdit->setText(value);
	}

	void setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const override
	{
		QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
		QString value = lineEdit->text();
		model->setData(index, value, Qt::EditRole);
	}
};
