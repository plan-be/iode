#pragma once

#include "abstract_delegate.h"


class PopupDelegate : public AbstractDelegate
{
	Q_OBJECT

public:
	PopupDelegate(EnumIodeCase nameCase, QObject* parent = nullptr) : AbstractDelegate(nameCase, parent) {}

	~PopupDelegate() {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override
	{
		if (index.column() == 0)
		{
			return createNameEditor(parent);
		}
		else
		{
			return nullptr;
		}
	}
};
