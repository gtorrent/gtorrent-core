#ifndef GTKTORRENTTREEVIEW_HPP
#define GTKTORRENTTREEVIEW_HPP

#include <gtkmm/cellrendererprogress.h>
#include "GtkTorrentColumns.hpp"
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <core/Torrent.hpp>

class GtkTorrentTreeView : public Gtk::TreeView
{
private:
	GtkTorrentColumns m_cols;
	Glib::RefPtr<Gtk::ListStore> m_liststore;
	void setupColumns();
public:
	GtkTorrentTreeView();

	void addCell(t_ptr &t);
	void updateCells();

};

#endif