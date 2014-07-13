#include "Gtk.hpp"
#include <gtkmm/application.h>
#include "GtkMainWindow.hpp"

GuiGtk::GuiGtk(int argc, char **argv)
{
	m_gtkmain = new Gtk::Main(0, 0, false);

	GtkMainWindow *wnd = Gtk::manage(new GtkMainWindow());
}

void GuiGtk::update()
{
	while (m_gtkmain->events_pending())
	{
		m_gtkmain->iteration();
	}
}