#pragma once

#include "text_delegate.h"


class CommentsDelegate : public TextDelegate
{
	Q_OBJECT

public:
	CommentsDelegate(QObject* parent = nullptr) : TextDelegate(ASIS, parent) {}

	~CommentsDelegate() {}
};
