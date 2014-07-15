#pragma once

#include <gui/core/Base.hpp>
#include <gtkmm/settings.h>
#include <gtkmm/main.h>
#include <memory>

class GuiGtk : public GuiBase
{
private:
	Gtk::Main *m_gtkmain;

public:
	GuiGtk(int argc, char **argv);

	/* Public Members & Functions */

	void update();
};

