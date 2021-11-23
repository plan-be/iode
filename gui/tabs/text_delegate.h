#pragma once

#include "abstract_delegate.h"


class TextDelegate : public AbstractDelegate
{
	Q_OBJECT

public:
	TextDelegate(QObject* parent = nullptr) : AbstractDelegate(true, parent) {}

	~TextDelegate() {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override
	{
		if (index.column() == 0)
		{
			return createNameEditor(parent);
		}
		else
		{
			return createTextEditor(parent);
		}
	}
};
