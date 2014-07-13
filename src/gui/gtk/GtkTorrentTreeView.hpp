#ifndef GTKTORRENTTREEVIEW_HPP
#define GTKTORRENTTREEVIEW_HPP

#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <core/Torrent.hpp>

/* Gtk Torrent Columns Section */

class GtkTorrentColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkTorrentColumns()
	{
		add(m_col_name);
		add(m_col_seeders);
		add(m_col_percent);
		add(m_col_percent_text);
	}
	
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<unsigned int> m_col_seeders;
	Gtk::TreeModelColumn<unsigned int> m_col_leechers;
	Gtk::TreeModelColumn<unsigned int> m_col_percent;
	Gtk::TreeModelColumn<Glib::ustring> m_col_percent_text;
	Gtk::TreeModelColumn<unsigned int> m_col_empty;
};

/* Gtk Torrent Tree View Section */

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