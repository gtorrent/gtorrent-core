#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include "lib/core/Core.hpp"
#include <memory>

class Application;	typedef std::shared_ptr<Application> app_ptr;

class Application
{
private:
	static app_ptr m_app;

	/* Private Member Functions & Variables */
	core_ptr m_core;
public:
	Application();
	static app_ptr getSingleton();

	/* Public Member Functions & Variables */
	int run(int argc, char **argv);
};
#endif