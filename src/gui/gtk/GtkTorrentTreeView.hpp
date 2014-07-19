#pragma once

#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <core/Torrent.hpp>

// Gtk Torrent Columns Section

class GtkTorrentColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkTorrentColumns(unsigned int flags = 0xFFFFFFFF)
	{
		if(flags &  1) add(m_col_name);
		if(flags &  2) add(m_col_seeders);
		if(flags &  4) add(m_col_leechers);
		if(flags &  8) add(m_col_dl_speed);
		if(flags & 16) add(m_col_percent);
		if(flags & 32) add(m_col_percent_text);
	}
	
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<unsigned int>  m_col_seeders;
	Gtk::TreeModelColumn<unsigned int>  m_col_leechers;
	Gtk::TreeModelColumn<unsigned int>  m_col_percent;
	Gtk::TreeModelColumn<Glib::ustring> m_col_percent_text;
	Gtk::TreeModelColumn<unsigned int>  m_col_empty;
	Gtk::TreeModelColumn<Glib::ustring> m_col_dl_speed;
};

// Gtk Torrent Tree View Section

class GtkTorrentTreeView : public Gtk::TreeView
{
private:
	GtkTorrentColumns m_cols;
	Glib::RefPtr<Gtk::ListStore> m_liststore;
	unsigned visibleColumns = 0xFFFFFFFF;

	void setupColumns();

	bool                      view_onClick(GdkEventButton *event);
	bool            torrentColumns_onClick(GdkEventButton *event);
	void            torrentContext_onClick();
	void               nameContext_onClick();
	void            seedersContext_onClick();
	void           leechersContext_onClick();
	void               rateContext_onClick();
	void           progressContext_onClick();
	void Rebuild();
	
public:
	GtkTorrentTreeView();

	void addCell(shared_ptr<Torrent> &t);
	void updateCells();
};

