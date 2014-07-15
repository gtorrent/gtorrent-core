#include "Gtk.hpp"

GuiGtk::GuiGtk(int argc, char **argv)
{
	m_gtkmain = new Gtk::Main(0, 0, false);

	mainWindow = new GtkMainWindow();
}

GuiGtk::~GuiGtk()
{
	// You know, we could use auto and
	// make_shared(...) instead so we still
	// receive benefits of shared_ptr
	// without the xbauxhueg mess

	// This is called after Core::shutdown

	delete mainWindow;
}

void GuiGtk::update()
{
	while (m_gtkmain->events_pending()) {
		m_gtkmain->iteration();
	}
}