#pragma once

#include <QObject>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QStyledItemDelegate>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "cpp_api/iode_cpp_api.h"
#include "utils.h"
#include "util/double_validator.h"


class BaseDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	EnumIodeType iodeType;

public:
	BaseDelegate(EnumIodeType iodeType, QObject* parent = nullptr) : QStyledItemDelegate(parent), iodeType(iodeType) {}

	~BaseDelegate() {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		if(iodeType == I_SCALARS || iodeType == I_VARIABLES)
		{
			QLineEdit* editor = new QLineEdit(parent);
			return editor;
		}
		else
		{
			QPlainTextEdit* editor = new QPlainTextEdit(parent);
			return editor;
		}
	}

	void setEditorData(QWidget* editor, const QModelIndex& index) const override
	{
		QString value = index.model()->data(index, Qt::EditRole).toString();
		if(iodeType == I_SCALARS || iodeType == I_VARIABLES)
		{
			QLineEdit* textEdit = static_cast<QLineEdit*>(editor);
			textEdit->setText(value);
		}
		else
		{
			QPlainTextEdit* textEdit = static_cast<QPlainTextEdit*>(editor);
			textEdit->setPlainText(value);
		}
	}

	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
	{
		QString value;
		if(iodeType == I_SCALARS || iodeType == I_VARIABLES)
		{
			QLineEdit* textEdit = static_cast<QLineEdit*>(editor);
			value = textEdit->text();
		}
		else
		{
			QPlainTextEdit* textEdit = static_cast<QPlainTextEdit*>(editor);
			value = textEdit->toPlainText();
		}
		model->setData(index, value, Qt::EditRole);
	}

protected:
	/**
	 * @brief Reimplement QStyledItemDelegate::eventFilter(). 
	 *        Consider the combination CTRL + ENTER as signal to commit changes and close the editor.    
	 *  
	 * @note see https://doc.qt.io/qt-6/qstyleditemdelegate.html#eventFilter
	 * 
	 * @param editor 
	 * @param event 
	 * @return true 
	 * @return false 
	 */
	bool eventFilter(QObject *editor, QEvent *event) override
	{
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if(keyEvent->modifiers() == Qt::ControlModifier && (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return))
			{
				commitData(static_cast<QWidget*>(editor));
				closeEditor(static_cast<QWidget*>(editor));
				return true;
			}
		}
		return QStyledItemDelegate::eventFilter(editor, event);
	}
};
