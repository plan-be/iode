#pragma once

#include "popup_delegate.h"


class EquationsDelegate : public PopupDelegate
{
	Q_OBJECT

public:
	EquationsDelegate(QObject* parent = nullptr) : PopupDelegate(I_UPPER, parent) {}

	~EquationsDelegate() {}
};
