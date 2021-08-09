#pragma once

#include "../bridge.h"
#include "../utils.h"

template <class M> class AbstractView
{
public:
	virtual void setupModel(M* model) = 0;
	virtual void update() = 0;
};

