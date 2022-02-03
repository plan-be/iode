#pragma once

#include "abstract_delegate.h"


class TextDelegate : public AbstractDelegate
{
	Q_OBJECT

public:
	TextDelegate(EnumIodeCase nameCase, QObject* parent = nullptr) : AbstractDelegate(nameCase, parent) {}

	~TextDelegate() {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override
	{
		if (index.column() == 0)
		{
			return createNameEditor(parent);
		}
		else
		{
			return createQLineEditor(parent);
		}
	}
};
