#pragma once

#include "text_delegate.h"


class TablesDelegate : public TextDelegate
{
	Q_OBJECT

public:
	TablesDelegate(QObject* parent = nullptr) : TextDelegate(I_UPPER, parent) {}

	~TablesDelegate() {}
};
