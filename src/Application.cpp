#include "Application.hpp"
#include <stdio.h>

app_ptr Application::m_App = nullptr;

Application::Application()
{
	/* Constructor */
}

app_ptr Application::getSingleton()
{
	if (m_App == nullptr)
		m_App = app_ptr(new Application());
	return m_App;
}

int Application::run(int argc, char **argv)
{
	printf("Hello, /g/!\n");
	return 0;
}