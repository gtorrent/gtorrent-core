#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/treeviewcolumn.h>
#include <gtkmm/hvseparator.h>
#include <gtkmm/checkmenuitem.h>

#include <Application.hpp>
#include "GtkTorrentTreeView.hpp"

GtkTorrentTreeView::GtkTorrentTreeView()
{
	m_liststore = Gtk::ListStore::create(m_cols);
	this->set_model(m_liststore);
	this->setupColumns();
	signal_button_press_event().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::torrentView_onClick), false);

	int i = 1;
	for(auto c : get_columns())
	{
		Gtk::CheckMenuItem *rcmItem1 = Gtk::manage(new Gtk::CheckMenuItem(c->get_title()));
		rcmItem1->set_active();
		rcmItem1->signal_button_press_event().connect(sigc::bind<1>(sigc::mem_fun(*this, &GtkTorrentTreeView::ColumnContextMenu_onClick), i));
		i <<= 1;
		m_rcMenu->add(*rcmItem1);
	}
}

bool GtkTorrentTreeView::torrentView_onClick(GdkEventButton *event)
{
	if(event->button == 3)
	{
		Gtk::Menu     *m_rcMenu   = Gtk::manage(new Gtk::Menu());
		Gtk::MenuItem *rcmItem1 = Gtk::manage(new Gtk::MenuItem("Start"));
		Gtk::MenuItem *rcmItem2 = Gtk::manage(new Gtk::MenuItem("Stop"));
		Gtk::MenuItem *rcmItem3 = Gtk::manage(new Gtk::MenuItem("Remove"));
		/* If someone finds out how to put a horizontal separator in the menu like on web browsers  put it here */
		Gtk::MenuItem *rcmItem4 = Gtk::manage(new Gtk::MenuItem("Open"));
		Gtk::MenuItem *rcmItem5 = Gtk::manage(new Gtk::MenuItem("Priority")); // Also if you find a way to expand another menu from there
		/* If someone finds out how to put a horizontal separator in the menu like on web browsers  put it here */
		Gtk::MenuItem *rcmItem6 = Gtk::manage(new Gtk::MenuItem("Property"));

		rcmItem1->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::startView_onClick));
		rcmItem2->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::stopView_onClick));
		rcmItem3->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::removeView_onClick));
		rcmItem4->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::openView_onClick));

		/* Maybe an onHover or smth for this one. */
		rcmItem5->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::priorityView_onClick));
		rcmItem6->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::propertyView_onClick));

		m_rcMenu->add(*rcmItem1);
		m_rcMenu->add(*rcmItem2);
		m_rcMenu->add(*rcmItem3);
		m_rcMenu->add(*rcmItem4);
		m_rcMenu->add(*rcmItem5);
		m_rcMenu->add(*rcmItem6);

		m_rcMenu->show_all();
		m_rcMenu->popup(event->button, event->time);
	}

	return false;
}

bool GtkTorrentTreeView::ColumnContextMenu_onClick(GdkEventButton *event, int n)
{
	m_visibleColumns ^= n;
	remove_all_columns();
	setupColumns();
	return true;
}

bool GtkTorrentTreeView::torrentColumns_onClick(GdkEventButton *event)
{
	if(event->button == 3)
	{
		m_rcMenu->show_all();
		m_rcMenu->popup(event->button, event->time);
	}

	return true; //The bool that determine if the event has been handled allows to propagete or not a click
}

void GtkTorrentTreeView::setupColumns()
{
	unsigned int cid = 0;
	Gtk::TreeViewColumn *col = nullptr;
	Gtk::CellRendererProgress *cell = nullptr;

	if(m_visibleColumns & 1)
	{
		cid = this->append_column("Queue", m_cols.m_col_queue);
		col = this->get_column(cid - 1);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 2)
	{
		cid = this->append_column("Name", m_cols.m_col_name);
		col = this->get_column(cid - 1);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 4)
	{
		cid = this->append_column("Active", m_cols.m_col_active);
		col = this->get_column(cid - 1);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 8)
	{
		cid = this->append_column("ETA", m_cols.m_col_eta);
		col = this->get_column(cid - 1);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 16)
	{
		cid = this->append_column("Seeders", m_cols.m_col_seeders);
		col = this->get_column(cid - 1);
		col->set_alignment(0.5);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 32)
	{
		cid = this->append_column("Leechers", m_cols.m_col_leechers);
		col = this->get_column(cid - 1);
		col->set_alignment(0.5);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 64)
	{
		cid = this->append_column("Rate", m_cols.m_col_dl_speed);
		col = this->get_column(cid - 1);
		col->set_alignment(0.5);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 128)
	{
		cid = this->append_column("Upspeed", m_cols.m_col_ul_speed);
		col = this->get_column(cid - 1);
		col->set_alignment(0.5);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 256)
	{
		cid = this->append_column("Downspeed", m_cols.m_col_dl_speed);
		col = this->get_column(cid - 1);
		col->set_alignment(0.5);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 512)
	{
		cid = this->append_column("Uploaded", m_cols.m_col_ul_total);
		col = this->get_column(cid - 1);
		col->set_alignment(0.5);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 1024)
	{
		cid = this->append_column("Downloaded", m_cols.m_col_dl_total);
		col = this->get_column(cid - 1);
		col->set_alignment(0.5);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 2048)
	{
		cid = this->append_column("Size", m_cols.m_col_size);
		col = this->get_column(cid - 1);
		col->set_alignment(0.5);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 4096)
	{
		cid = this->append_column("Remaining", m_cols.m_col_remaining);
		col = this->get_column(cid - 1);
		col->set_alignment(0.5);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 8192)
	{
		cid = this->append_column("Ratio", m_cols.m_col_dl_ratio);
		col = this->get_column(cid - 1);
		col->set_alignment(0.5);
		col->set_fixed_width(128);
	}

	if(m_visibleColumns & 16384)
	{
		cell = Gtk::manage(new Gtk::CellRendererProgress());
		cid = this->append_column("Progress", *cell);
		col = this->get_column(cid - 1);
		col->add_attribute(cell->property_value(), m_cols.m_col_percent);
		col->add_attribute(cell->property_text(), m_cols.m_col_percent_text);
	}

	for (auto &c : this->get_columns())
	{
		Gtk::Button *butt = c->get_button();
		butt->signal_button_press_event().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::torrentColumns_onClick));
		c->set_sizing(Gtk::TreeViewColumnSizing::TREE_VIEW_COLUMN_FIXED);
		c->set_clickable();
		c->set_resizable();
		c->set_reorderable();
	}
}

void GtkTorrentTreeView::addCell(shared_ptr<Torrent> &t)
{
	if (t == NULL)
		return;

	Gtk::TreeModel::Row row = *(m_liststore->append());
	row[m_cols.m_col_active] = t->getTextActive();
	row[m_cols.m_col_eta] = t->getTextEta();
	row[m_cols.m_col_name] = t->getHandle().name();
	row[m_cols.m_col_percent] = t->getTotalProgress();
	row[m_cols.m_col_percent_text] = t->getTextState();
	row[m_cols.m_col_seeders] = t->getTotalSeeders();
	row[m_cols.m_col_leechers] = t->getTotalLeechers();
	row[m_cols.m_col_ul_total] = t->getTextTotalUploaded();
	row[m_cols.m_col_dl_total] = t->getTextTotalDownloaded();
	row[m_cols.m_col_size] = t->getTextSize();
	row[m_cols.m_col_remaining] = t->getTextRemaining();
	row[m_cols.m_col_dl_ratio] = t->getTextTotalRatio();
}

void GtkTorrentTreeView::updateCells()
{
	unsigned int i = 0;
	for (auto &c : m_liststore->children())
	{
		shared_ptr<Torrent> t = Application::getSingleton()->getCore()->getTorrents()[i];

		c[m_cols.m_col_active] = t->getTextActive();
		c[m_cols.m_col_eta] = t->getTextEta();
		c[m_cols.m_col_percent] = t->getTotalProgress();
		c[m_cols.m_col_seeders] = t->getTotalSeeders();
		c[m_cols.m_col_percent_text] = t->getTextState();
		c[m_cols.m_col_leechers] = t->getTotalLeechers();
		c[m_cols.m_col_ul_speed] = t->getTextUploadRate();
		c[m_cols.m_col_dl_speed] = t->getTextDownloadRate();
		c[m_cols.m_col_ul_total] = t->getTextTotalUploaded();
		c[m_cols.m_col_dl_total] = t->getTextTotalDownloaded();
		c[m_cols.m_col_size] = t->getTextSize();
		c[m_cols.m_col_dl_ratio] = t->getTextTotalRatio();
		c[m_cols.m_col_eta] = t->getTextTimeRemaining();

		// TODO: Handle with events

		//m_cells[i]->property_text() = t->getTextState();

		++i;
	}
}

void GtkTorrentTreeView::    stopView_onClick() { /* Doesn't do nuffin wrong */ }
void GtkTorrentTreeView::    openView_onClick() { /* Doesn't do nuffin wrong */ }
void GtkTorrentTreeView::   startView_onClick() { /* Doesn't do nuffin wrong */ }
void GtkTorrentTreeView::  removeView_onClick() { /* Doesn't do nuffin wrong */ }
void GtkTorrentTreeView::priorityView_onClick() { /* Doesn't do nuffin wrong */ }
void GtkTorrentTreeView::propertyView_onClick() { /* Doesn't do nuffin wrong */ }
