#pragma once

#include "base_delegate.h"


class ListsDelegate : public BaseDelegate
{
	Q_OBJECT

public:
	ListsDelegate(QObject* parent = nullptr) : BaseDelegate(LISTS, parent) {}

	~ListsDelegate() {}
};
