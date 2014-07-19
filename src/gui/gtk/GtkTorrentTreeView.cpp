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

void GtkTorrentTreeView::torrentContext_onClick()
{
	puts(">2014\n>touching my tralala");
}


void GtkTorrentTreeView::columnContext_onClick()
{
	puts(">2014\n>touching my dingdingdong");
}

bool GtkTorrentTreeView::torrentColumns_onClick(GdkEventButton *event)
{
	if(event->button == 3) 
	{
		Gtk::Menu     *rcMenu  = Gtk::manage(new Gtk::Menu());
		Gtk::MenuItem *rcmItem = Gtk::manage(new Gtk::MenuItem("This is a click on a torrent column"));
		rcmItem->signal_activate().connect(sigc::mem_fun(*this, &GtkTorrentTreeView::columnContext_onClick));
		rcMenu->add(*rcmItem);
		rcMenu->show_all();
		rcMenu->popup(event->button, event->time);
	}

	return true; //The bool that determine if the event has been handled allows to propagete or not a click
}

void GtkTorrentTreeView::setupColumns()
{
	unsigned int cid = 0;
	Gtk::TreeViewColumn *col = nullptr;

	cid = this->append_column("Name", m_cols.m_col_name);
	col = this->get_column(cid - 1);
	col->set_fixed_width(250);

	cid = this->append_column("Seeders", m_cols.m_col_seeders);
	col = this->get_column(cid - 1);
	col->set_alignment(0.5);
	col->set_fixed_width(90);
	
	cid = this->append_column("Leechers", m_cols.m_col_leechers);
	col = this->get_column(cid - 1);
	col->set_alignment(0.5);
	col->set_fixed_width(90);

	cid = this->append_column("Rate", m_cols.m_col_dl_speed);
	col = this->get_column(cid - 1);
	col->set_alignment(0.5);
	col->set_fixed_width(95);

	Gtk::CellRendererProgress *cell = Gtk::manage(new Gtk::CellRendererProgress());
	cid = this->append_column("Progress", *cell);
	col = this->get_column(cid - 1);

	if (col) 
	{
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

	for (auto &c : m_liststore->children()) {
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

