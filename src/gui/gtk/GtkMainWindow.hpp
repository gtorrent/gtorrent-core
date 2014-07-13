#ifndef GTKMAINWINDOW_HPP
#define GTKMAINWINDOW_HPP

#include "GtkTorrentTreeView.hpp"
#include <gtkmm/headerbar.h>
#include <core/Core.hpp>
#include "GtkWindow.hpp"

class GtkMainWindow : public gt::GtkWindow
{
private:
	Gtk::HeaderBar *header;
	GtkTorrentTreeView *m_treeview;
public:
	GtkMainWindow();

	/* Signal Responders */
	void onAddBtnClicked();
	void onAddMagnetBtnClicked();

	bool onDestroy(GdkEventAny *event);
	bool onSecTick();
};

#endif