#ifndef GUIGTK_HPP
#define GUIGTK_HPP

#include <gui/core/Base.hpp>
#include <gtkmm/settings.h>
#include <gtkmm/main.h>

class GuiGtk : public GuiBase
{
private:
	Gtk::Main *m_gtkmain;
public:
	GuiGtk(int argc, char **argv);

	void update();
};

#endif