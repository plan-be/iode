#pragma once

#include "base_delegate.h"


class IdentitiesDelegate : public BaseDelegate
{
	Q_OBJECT

public:
	IdentitiesDelegate(QObject* parent = nullptr) : BaseDelegate(IDENTITIES, parent) {}

	~IdentitiesDelegate() {}
};

