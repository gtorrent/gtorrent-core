#include <gtkmm/cellrendererprogress.h>
#include <gtkmm/treeviewcolumn.h>
#include "GtkTorrentTreeView.hpp"
#include <core/OSHelper.hpp>
#include <Application.hpp>

GtkTorrentTreeView::GtkTorrentTreeView()
{
	m_liststore = Gtk::ListStore::create(m_cols);
	this->set_model(m_liststore);
	this->setupColumns();
}

void GtkTorrentTreeView::setupColumns()
{
	this->append_column("Name", m_cols.m_col_name);
	this->append_column("Seeders", m_cols.m_col_seeders);

	Gtk::CellRendererProgress *cell = Gtk::manage(new Gtk::CellRendererProgress());
	int c = this->append_column("Progress", *cell);
	Gtk::TreeViewColumn *p_col = this->get_column(c - 1);

	if (p_col)
		p_col->add_attribute(cell->property_value(), m_cols.m_col_percent);

	for (unsigned int i = 0; i < 2; ++i)
	{
		Gtk::TreeView::Column *tc = this->get_column(i);
		tc->set_sizing(Gtk::TreeViewColumnSizing::TREE_VIEW_COLUMN_FIXED);
		tc->set_fixed_width(200);
		tc->set_resizable();
		tc->set_reorderable();
	}
}

void GtkTorrentTreeView::addCell(t_ptr &t)
{
	Gtk::TreeModel::Row row = *(m_liststore->append());
	row[m_cols.m_col_name] = t->getHandle().name();
	row[m_cols.m_col_percent] = t->getTotalProgress();
	row[m_cols.m_col_seeders] = t->getTotalSeeders();
}

void GtkTorrentTreeView::updateCells()
{
	unsigned int i = 0;

	for (auto &c : m_liststore->children())
	{
		t_ptr t = Application::getSingleton()->getCore()->getEngine()->getTorrents()[i];

		c[m_cols.m_col_percent] = t->getTotalProgress();
		c[m_cols.m_col_seeders] = t->getTotalSeeders();

		++i;
	}
}