#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/treeviewcolumn.h>
#include <gtkmm/hvseparator.h>
#include "GtkTorrentTreeView.hpp"
#include <Application.hpp>

GtkTorrentTreeView::GtkTorrentTreeView()
{
	m_liststore = Gtk::ListStore::create(m_cols);
	this->set_model(m_liststore);
	this->setupColumns();
	signal_button_press_event().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::view_onClick), false);
}

bool GtkTorrentTreeView::view_onClick(GdkEventButton *event)
{
	if(event->button == 3) 
	{
		Gtk::Menu     *rcMenu  = Gtk::manage(new Gtk::Menu());
		Gtk::MenuItem *rcmItem = Gtk::manage(new Gtk::MenuItem("This is a click on the torrent view"));
		rcmItem->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::torrentContext_onClick));
		rcMenu->add(*rcmItem);
		rcMenu->show_all();
		rcMenu->popup(event->button, event->time);
	}

	return false;
}

bool GtkTorrentTreeView::torrentColumns_onClick(GdkEventButton *event)
{
	if(event->button == 3) 
	{
		Gtk::Menu     *rcMenu  = Gtk::manage(new Gtk::Menu());

		Gtk::MenuItem *rcmItem1 = Gtk::manage(new Gtk::MenuItem("Name"));
		Gtk::MenuItem *rcmItem2 = Gtk::manage(new Gtk::MenuItem("Seeders"));
		Gtk::MenuItem *rcmItem3 = Gtk::manage(new Gtk::MenuItem("Leechers"));
		Gtk::MenuItem *rcmItem4 = Gtk::manage(new Gtk::MenuItem("Rate"));
		Gtk::MenuItem *rcmItem5 = Gtk::manage(new Gtk::MenuItem("Progress"));

		rcmItem1->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::    nameContext_onClick));
		rcmItem4->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::    rateContext_onClick));
		rcmItem2->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView:: seedersContext_onClick));
		rcmItem3->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::leechersContext_onClick));
		rcmItem5->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::progressContext_onClick));

		rcMenu->add(*rcmItem1);
		rcMenu->add(*rcmItem2);
		rcMenu->add(*rcmItem3);
		rcMenu->add(*rcmItem4);
		rcMenu->add(*rcmItem5);

		rcMenu->show_all();
		rcMenu->popup(event->button, event->time);
	}

	return true; //The bool that determine if the event has been handled allows to propagete or not a click
}

void GtkTorrentTreeView::setupColumns()
{
	unsigned int cid = 0;
	Gtk::TreeViewColumn *col = nullptr;
	Gtk::CellRendererProgress *cell = nullptr;
	if(visibleColumns & 1)
	{
		cid = this->append_column("Name", m_cols.m_col_name);
		col = this->get_column(cid - 1);
		col->set_fixed_width(250);
	}

	if(visibleColumns & 2)
	{
		cid = this->append_column("Seeders", m_cols.m_col_seeders);
		col = this->get_column(cid - 1);
		col->set_alignment(0.5);
		col->set_fixed_width(90);
	}

	if(visibleColumns & 4)
	{
		cid = this->append_column("Leechers", m_cols.m_col_leechers);
		col = this->get_column(cid - 1);
		col->set_alignment(0.5);
		col->set_fixed_width(90);
	}

	if(visibleColumns & 8)
	{
		cid = this->append_column("Rate", m_cols.m_col_dl_speed);
		col = this->get_column(cid - 1);
		col->set_alignment(0.5);
		col->set_fixed_width(95);
	}

	if(visibleColumns & 16)
	{
		cell = Gtk::manage(new Gtk::CellRendererProgress());
		cid = this->append_column("Progress", *cell);
		col = this->get_column(cid - 1);
		col->add_attribute(cell->property_value(), m_cols.m_col_percent);
		col->add_attribute(cell->property_text(), m_cols.m_col_percent_text);
	}

	for (auto &c : this->get_columns()) 
	{
		c->set_sizing(Gtk::TreeViewColumnSizing::TREE_VIEW_COLUMN_FIXED);

		Gtk::Button *butt = c->get_button();	
		butt->signal_button_press_event().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::torrentColumns_onClick));

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
	row[m_cols.m_col_name] = t->getHandle().name();
	row[m_cols.m_col_percent] = t->getTotalProgress();
	row[m_cols.m_col_percent_text] = t->getTextState();
	row[m_cols.m_col_seeders] = t->getTotalSeeders();
	row[m_cols.m_col_leechers] = t->getTotalLeechers();

}

void GtkTorrentTreeView::updateCells()
{
	unsigned int i = 0;
	for (auto &c : m_liststore->children())
	{
		shared_ptr<Torrent> t = Application::getSingleton()->getCore()->getTorrents()[i];

		c[m_cols.m_col_percent] = t->getTotalProgress();
		c[m_cols.m_col_seeders] = t->getTotalSeeders();
		c[m_cols.m_col_percent_text] = t->getTextState();
		c[m_cols.m_col_leechers] = t->getTotalLeechers();
		c[m_cols.m_col_dl_speed] = t->getTextDownloadRate();
		
		// TODO: Handle with events

		//m_cells[i]->property_text() = t->getTextState();

		++i;
	}
}

void GtkTorrentTreeView::torrentContext_onClick()
{
//	insert_column(m_cols.m_col_percent, 4);
}


void GtkTorrentTreeView::nameContext_onClick()
{
	visibleColumns ^= 1;
	Rebuild();
}

void GtkTorrentTreeView::seedersContext_onClick()
{
	visibleColumns ^= 2;
	Rebuild();
}

void GtkTorrentTreeView::leechersContext_onClick()
{
	visibleColumns ^= 4;
	Rebuild();
}

void GtkTorrentTreeView::rateContext_onClick()
{
	visibleColumns ^= 8;
	Rebuild();
}

void GtkTorrentTreeView::progressContext_onClick()
{
	visibleColumns ^= 16;
	Rebuild();
}

void GtkTorrentTreeView::Rebuild()
{
	remove_all_columns();
	setupColumns();
}
