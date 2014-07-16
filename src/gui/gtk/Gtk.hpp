#pragma once

#include <gui/core/Base.hpp>
#include "GtkMainWindow.hpp"
#include <gtkmm/settings.h>
#include <gtkmm/main.h>
#include <memory>

class GuiGtk : public GuiBase
{
private:
	Gtk::Main *m_gtkmain;
	GtkMainWindow *mainWindow;
public:
	GuiGtk();
	~GuiGtk();

	// Public Members & Functions

	void update();
};

