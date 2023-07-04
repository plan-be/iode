#pragma once

#include "base_delegate.h"


class EquationsDelegate : public BaseDelegate
{
	Q_OBJECT

public:
	EquationsDelegate(QObject* parent = nullptr) : BaseDelegate(I_EQUATIONS, parent) {}

	~EquationsDelegate() {}
};
