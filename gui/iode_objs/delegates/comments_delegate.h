#pragma once

#include "base_delegate.h"


class CommentsDelegate : public BaseDelegate
{
	Q_OBJECT

public:
	CommentsDelegate(QObject* parent = nullptr) : BaseDelegate(I_ASIS, parent) {}

	~CommentsDelegate() {}
};
