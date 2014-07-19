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
		add(m_col_name);
		add(m_col_seeders);
		add(m_col_leechers);
		add(m_col_dl_speed);
		add(m_col_percent);
		add(m_col_percent_text);
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

	/* Yeah it's shit. */
	/* Just a quick way to toggle columns */
	/* Each bit from the LSB enables or disables a panel */
	/* You can toggle a bit by XORing with 1 */
	unsigned visibleColumns = 0xFFFFFFFF;

	void setupColumns();

	/* Event handlers for clicks on the controls */
	bool               torrentView_onClick(GdkEventButton *event);
	bool            torrentColumns_onClick(GdkEventButton *event);

	/* Event Handlers for the columns context Menu */
	void     nameColumnContext_onClick();
	void     rateColumnContext_onClick();
	void  seedersColumnContext_onClick();
	void  torrentColumnContext_onClick();
	void leechersColumnContext_onClick();
	void progressColumnContext_onClick();

	/* Event handlers for the torrent view context menu */
	void     stopView_onClick();
	void     openView_onClick();
	void    startView_onClick();
	void   removeView_onClick();
	void priorityView_onClick();
	void propertyView_onClick();

	void RebuildTorrentView();
	
public:
	GtkTorrentTreeView();

	void addCell(shared_ptr<Torrent> &t);
	void updateCells();
};

