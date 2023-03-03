#pragma once

#include "base_delegate.h"


class PopupDelegate : public BaseDelegate
{
	Q_OBJECT

public:
	PopupDelegate(EnumIodeCase nameCase, QObject* parent = nullptr) : BaseDelegate(nameCase, parent) {}

	~PopupDelegate() {}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override
	{
		return nullptr;
	}
};
