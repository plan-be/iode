#pragma once

#include "abstract_delegate.h"


class VariablesDelegate : public AbstractDelegate
{
	Q_OBJECT

public:
	VariablesDelegate(QObject* parent = nullptr) : AbstractDelegate(UPPER, parent) {}

	~VariablesDelegate() {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override
	{
		if (index.column() == 0)
		{
			return createNameEditor(parent);
		}
		else
		{
			QLineEdit* editor = new QLineEdit(parent);
			editor->setValidator(new IODEDoubleValidator(editor));
			return editor;
		}
	}
};
