#pragma once

#include "text_delegate.h"


class IdentitiesDelegate : public TextDelegate
{
	Q_OBJECT

public:
	IdentitiesDelegate(QObject* parent = nullptr) : TextDelegate(I_UPPER, parent) {}

	~IdentitiesDelegate() {}
};

