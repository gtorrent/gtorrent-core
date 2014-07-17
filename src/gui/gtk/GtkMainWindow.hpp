#pragma once

#include "GtkTorrentTreeView.hpp"
#include <gtkmm/headerbar.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <core/Core.hpp>
#include <gtkmm/main.h>

class GtkMainWindow : public Gtk::Window
{
private:
	shared_ptr<gt::Core> &m_core;

	Gtk::HeaderBar *header;
	GtkTorrentTreeView *m_treeview;

	Gtk::Button *pause_btn;

	// Signal Responders
	
	void onAddBtnClicked();
	void onPauseBtnClicked();
	void onAddMagnetBtnClicked();
	void onTreeViewRowActivated(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);
public:
	GtkMainWindow();

	bool onDestroy(GdkEventAny *event);
	bool onSecTick();
};

