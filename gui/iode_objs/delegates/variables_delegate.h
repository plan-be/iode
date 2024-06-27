#pragma once

#include "base_delegate.h"


class VariablesDelegate : public BaseDelegate
{
	Q_OBJECT

public:
	VariablesDelegate(QObject* parent = nullptr) : BaseDelegate(VARIABLES, parent) {}

	~VariablesDelegate() {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override
	{
		QLineEdit* editor = new QLineEdit(parent);
		editor->setValidator(new IODEDoubleValidator(editor));
		return editor;
	}
};
