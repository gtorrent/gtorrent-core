#pragma once

#include "core/Core.hpp"
#include <memory>

class GuiBase
{
protected:
	shared_ptr<gt::Core> &m_core;

public:
	GuiBase();
	virtual void update() = 0;
};

