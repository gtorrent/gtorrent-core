#ifndef GUIBASE_HPP
#define GUIBASE_HPP
#include <string>

class GuiBase
{
protected:
	virtual std::string getOpenFileDialog() = 0;
	virtual std::string getSaveFileDialog() = 0;
};

#endif