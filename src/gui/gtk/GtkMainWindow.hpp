#ifndef GTKMAINWINDOW_HPP
#define GTKMAINWINDOW_HPP

#include "GtkTorrentTreeView.hpp"
#include <gtkmm/headerbar.h>
#include <gtkmm/window.h>
#include <core/Core.hpp>
#include <gtkmm/main.h>

class GtkMainWindow : public Gtk::Window
{
private:
	core_ptr &m_core;

	Gtk::HeaderBar *header;
	GtkTorrentTreeView *m_treeview;

	bool isMagnetLink(std::string str);

	/* Signal Responders */
	void onAddBtnClicked();
	void onAddMagnetBtnClicked();
	void onClipboardReady(const Glib::ustring &text);

public:
	GtkMainWindow();

	bool onDestroy(GdkEventAny *event);
	bool onSecTick();
};

#endif