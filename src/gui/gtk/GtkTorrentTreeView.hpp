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
	GtkTorrentColumns()
	{
		add(m_col_active);
		add(m_col_name);
		add(m_col_seeders);
		add(m_col_leechers);
		add(m_col_ul_speed);
		add(m_col_dl_speed);
		add(m_col_percent);
		add(m_col_percent_text);
		add(m_col_ul_total);
		add(m_col_dl_total);
		add(m_col_size);
		add(m_col_dl_ratio);
	}
	Gtk::TreeModelColumn<Glib::ustring> m_col_active;
	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	Gtk::TreeModelColumn<unsigned int> m_col_seeders;
	Gtk::TreeModelColumn<unsigned int> m_col_leechers;
	Gtk::TreeModelColumn<unsigned int> m_col_percent;
	Gtk::TreeModelColumn<Glib::ustring> m_col_percent_text;
	Gtk::TreeModelColumn<unsigned int> m_col_empty;
	Gtk::TreeModelColumn<Glib::ustring> m_col_ul_speed;
	Gtk::TreeModelColumn<Glib::ustring> m_col_dl_speed;
	Gtk::TreeModelColumn<Glib::ustring> m_col_ul_total;
	Gtk::TreeModelColumn<Glib::ustring> m_col_dl_total;
	Gtk::TreeModelColumn<Glib::ustring> m_col_size;
	Gtk::TreeModelColumn<Glib::ustring> m_col_dl_ratio;
};

// Gtk Torrent Tree View Section

class GtkTorrentTreeView : public Gtk::TreeView
{
private:
	GtkTorrentColumns m_cols;
	Glib::RefPtr<Gtk::ListStore> m_liststore;
	void setupColumns();

public:
	GtkTorrentTreeView();

	void addCell(shared_ptr<Torrent> &t);
	void updateCells();
};

