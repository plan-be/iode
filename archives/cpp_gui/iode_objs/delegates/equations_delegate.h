#pragma once

#include "base_delegate.h"


class EquationsDelegate : public BaseDelegate
{
	Q_OBJECT

public:
	EquationsDelegate(QObject* parent = nullptr) : BaseDelegate(EQUATIONS, parent) {}

	~EquationsDelegate() {}
};
