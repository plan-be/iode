#pragma once

#include "text_delegate.h"


class CommentsDelegate : public TextDelegate
{
	Q_OBJECT

public:
	CommentsDelegate(QObject* parent = nullptr) : TextDelegate(I_ASIS, parent) {}

	~CommentsDelegate() {}
};
