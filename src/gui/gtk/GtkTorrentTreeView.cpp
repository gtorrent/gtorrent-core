#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/treeviewcolumn.h>
#include "GtkTorrentTreeView.hpp"
#include <Application.hpp>

GtkTorrentTreeView::GtkTorrentTreeView()
{
	m_liststore = Gtk::ListStore::create(m_cols);
	this->set_model(m_liststore);
	this->setupColumns();
}

void GtkTorrentTreeView::setupColumns()
{
	unsigned int cid = 0;
	Gtk::TreeViewColumn *col = nullptr;

	cid = this->append_column("Name", m_cols.m_col_name);
	col = this->get_column(cid - 1);
	col->set_fixed_width(325);

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

	if (col) {
		col->add_attribute(cell->property_value(), m_cols.m_col_percent);
		col->add_attribute(cell->property_text(), m_cols.m_col_percent_text);
	}

	for (auto &c : this->get_columns()) {
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

