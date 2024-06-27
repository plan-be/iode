#pragma once

#include "base_delegate.h"


class TablesDelegate : public BaseDelegate
{
	Q_OBJECT

public:
	TablesDelegate(QObject* parent = nullptr) : BaseDelegate(TABLES, parent) {}

	~TablesDelegate() {}
};
