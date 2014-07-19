#pragma once

#include <gui/core/Base.hpp>
#include "GtkMainWindow.hpp"
#include <gtkmm/settings.h>
#include <gtkmm/main.h>

class GuiGtk : public GuiBase
{
private:
	Gtk::Main *m_gtkmain;
	GtkMainWindow *mainWindow;
public:
	GuiGtk(int argc, char **argv);
	~GuiGtk();

	// Public Members & Functions

	void update();
};

