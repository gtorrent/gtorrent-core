#ifndef GTKMAINWINDOW_HPP
#define GTKMAINWINDOW_HPP

#include <gtkmm/headerbar.h>
#include <gtkmm/window.h>
#include <core/Core.hpp>

class GtkMainWindow : public Gtk::Window
{
private:
	Gtk::HeaderBar *header;

	core_ptr &m_core;
public:
	GtkMainWindow();

	/* Signal Responders */
	void onAddBtnClicked();
	bool onDestroy(GdkEventAny *event);
};

#endif