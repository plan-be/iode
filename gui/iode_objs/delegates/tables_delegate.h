#pragma once

#include "base_delegate.h"


class TablesDelegate : public BaseDelegate
{
	Q_OBJECT

public:
	TablesDelegate(QObject* parent = nullptr) : BaseDelegate(I_UPPER, parent) {}

	~TablesDelegate() {}
};
