#pragma once

#include "base_delegate.h"


class CommentsDelegate : public BaseDelegate
{
	Q_OBJECT

public:
	CommentsDelegate(QObject* parent = nullptr) : BaseDelegate(COMMENTS, parent) {}

	~CommentsDelegate() {}
};
