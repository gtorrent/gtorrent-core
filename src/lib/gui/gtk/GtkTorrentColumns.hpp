#ifndef GTKTORRENTCOLUMNS_HPP
#define GTKTORRENTCOLUMNS_HPP

#include <gtkmm/treemodel.h>

class GtkTorrentColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	GtkTorrentColumns();

	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<unsigned int> m_col_seeders;
	Gtk::TreeModelColumn<unsigned int> m_col_leechers;
	Gtk::TreeModelColumn<unsigned int> m_col_percent;
	Gtk::TreeModelColumn<Glib::ustring> m_col_percent_text;
	Gtk::TreeModelColumn<unsigned int> m_col_empty;
};

#endif