#pragma once

#include "popup_delegate.h"


class TablesDelegate : public PopupDelegate
{
	Q_OBJECT

public:
	TablesDelegate(QObject* parent = nullptr) : PopupDelegate(I_UPPER, parent) {}

	~TablesDelegate() {}
};
