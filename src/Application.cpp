#include "Application.hpp"

app_ptr Application::m_app = nullptr;

Application::Application()
{
	/* Constructor */
}

app_ptr Application::getSingleton()
{
	if (m_app == nullptr)
		m_app = app_ptr(new Application());
	return m_app;
}

int Application::run(int argc, char **argv)
{
	m_core = core_ptr(new GTorrent_Core());
	return 0;
}