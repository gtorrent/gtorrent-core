#include "Application.hpp"
#include <core/Log.hpp>

shared_ptr<Application> Application::m_app = nullptr;

shared_ptr<Application> Application::getSingleton()
{
	if (m_app == nullptr)
		m_app = shared_ptr<Application>(new Application());
	return m_app;
}

core_ptr &Application::getCore()
{
	return m_core;
}

int Application::run(int argc, char **argv)
{
	gt::Log::Debug("Starting up core library...\n");
	m_core = core_ptr(new Core());

	gt::Log::Debug("Starting up GUI layer...\n");
	m_gui = gui_ptr(new GuiGtk(argc, argv));

	while (m_core->isRunning()) {
		m_core->update();
		m_gui->update();
	}

	return 0;
}