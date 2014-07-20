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

	cid = this->append_column("Active", m_cols.m_col_active);
	col = this->get_column(cid - 1);
	col->set_fixed_width(84);

	cid = this->append_column("ETA", m_cols.m_col_eta);
	col = this->get_column(cid - 1);
	col->set_fixed_width(84);

	cid = this->append_column("Name", m_cols.m_col_name);
	col = this->get_column(cid - 1);
	col->set_fixed_width(84);

	cid = this->append_column("Seeders", m_cols.m_col_seeders);
	col = this->get_column(cid - 1);
	col->set_alignment(0.5);
	col->set_fixed_width(84);

	cid = this->append_column("Leechers", m_cols.m_col_leechers);
	col = this->get_column(cid - 1);
	col->set_alignment(0.5);
	col->set_fixed_width(96);

	cid = this->append_column("Up", m_cols.m_col_ul_speed);
	col = this->get_column(cid - 1);
	col->set_alignment(0.5);
	col->set_fixed_width(96);

	cid = this->append_column("Down", m_cols.m_col_dl_speed);
	col = this->get_column(cid - 1);
	col->set_alignment(0.5);
	col->set_fixed_width(102);

	cid = this->append_column("Uploaded", m_cols.m_col_ul_total);
	col = this->get_column(cid - 1);
	col->set_alignment(0.5);
	col->set_fixed_width(96);

	cid = this->append_column("Downloaded", m_cols.m_col_dl_total);
	col = this->get_column(cid - 1);
	col->set_alignment(0.5);
	col->set_fixed_width(112);

	cid = this->append_column("Size", m_cols.m_col_size);
        col = this->get_column(cid - 1);
        col->set_alignment(0.5);
        col->set_fixed_width(84);

	cid = this->append_column("Remaining", m_cols.m_col_remaining);
        col = this->get_column(cid - 1);
        col->set_alignment(0.5);
        col->set_fixed_width(96);

	cid = this->append_column("Ratio", m_cols.m_col_dl_ratio);
	col = this->get_column(cid - 1);
	col->set_alignment(0.5);
	col->set_fixed_width(84);


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

vector<unsigned> GtkTorrentTreeView::selectedIndices()
{
    Glib::RefPtr<Gtk::TreeSelection> sel = this->get_selection();
    sel->set_mode(Gtk::SelectionMode::SELECTION_MULTIPLE);
    vector<Gtk::TreeModel::Path> path = sel->get_selected_rows();
    vector<unsigned> indices;
    for (auto val : path)
        indices.push_back(val[0]); // we only get the first index because our tree is 1 node deep
    return indices;
}

void GtkTorrentTreeView::setSelectedPaused(bool isPaused)
{
    vector<shared_ptr<Torrent> > t = Application::getSingleton()->getCore()->getTorrents();
    for (auto i : selectedIndices())
        t[i]->setPaused(isPaused);// the pause button switches the status

}
