#ifndef GUIBASE_HPP
#define GUIBASE_HPP
#include <memory>
#include <string>

class GuiBase;	typedef std::shared_ptr<GuiBase> gui_ptr;

class GuiBase
{
protected:
	virtual std::string getOpenFileDialog() = 0;
	virtual std::string getSaveFileDialog() = 0;
	
public:
	virtual void update() = 0;
};

#endif