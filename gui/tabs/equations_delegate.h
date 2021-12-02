#pragma once

#include "text_delegate.h"


class EquationsDelegate : public TextDelegate
{
	Q_OBJECT

public:
	EquationsDelegate(QObject* parent = nullptr) : TextDelegate(UPPER, parent) {}

	~EquationsDelegate() {}
};

