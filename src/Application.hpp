#pragma once

#include <gui/core/Base.hpp>
#include <gui/gtk/Gtk.hpp>
#include <core/Core.hpp>

class Application
{
private:
	static shared_ptr<Application> m_app;
	
	// Private Member Functions & Variables 
	core_ptr m_core;
	gui_ptr m_gui;

public:
	static shared_ptr<Application> getSingleton();

	// Getters
	core_ptr &getCore();

	// Public Member Functions & Variables
	int run(int argc, char **argv);
};

