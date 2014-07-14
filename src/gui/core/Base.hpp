#ifndef GUIBASE_HPP
#define GUIBASE_HPP
#include "core/Core.hpp"
#include <memory>
#include <string>

class GuiBase;	typedef std::shared_ptr<GuiBase> gui_ptr;

class GuiBase
{
protected:
	core_ptr &m_core;
	
public:
	GuiBase();
	virtual void update() = 0;
};

#endif
