#include <Application.hpp>
#include "Base.hpp"

GuiBase::GuiBase() :
	m_core(Application::getSingleton()->getCore())
{
}