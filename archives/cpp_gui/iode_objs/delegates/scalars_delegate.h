#pragma once

#include "base_delegate.h"


class ScalarsDelegate : public BaseDelegate
{
	Q_OBJECT

public:
	ScalarsDelegate(QObject* parent = nullptr) : BaseDelegate(SCALARS, parent) {}

	~ScalarsDelegate() {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override
	{
		QLineEdit* editor = new QLineEdit(parent);

		switch (index.column())
		{
		case 0:
			editor->setValidator(new IODEDoubleValidator(editor));
			break;
		case 1:
			editor->setValidator(new QDoubleValidator(0., 1., 10, editor));
			break;
		default:
			editor = nullptr;
			break;
		}

		return editor;
	}
};
