#include "GtkAddMagnetLinkWindow.hpp"
#include <gtkmm/filechooserdialog.h>
#include "GtkMainWindow.hpp"
#include <Application.hpp>
#include <gtkmm/button.h>
#include <gtkmm/stock.h>
#include <glibmm.h>

GtkMainWindow::GtkMainWindow() :
	m_core(Application::getSingleton()->getCore())
{
	this->set_position(Gtk::WIN_POS_CENTER);
	this->set_default_size(800, 500);

	header = Gtk::manage(new Gtk::HeaderBar());
	header->set_title("gTorrent");
	header->set_show_close_button(true);

	// This needs to be refactored

	Gtk::Button *add_torrent_btn = Gtk::manage(new Gtk::Button());
	add_torrent_btn->set_label("Add");
	add_torrent_btn->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddBtnClicked));
	header->add(*add_torrent_btn);

	Gtk::Button *add_link_btn = Gtk::manage(new Gtk::Button());
	add_link_btn->set_label("Add Link");
	add_link_btn->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddMagnetBtnClicked));
	header->add(*add_link_btn);

	this->set_titlebar(*header);

	m_treeview = Gtk::manage(new GtkTorrentTreeView());
	this->add(*m_treeview);

	Glib::signal_timeout().connect(sigc::mem_fun(*this, &GtkMainWindow::onSecTick), 10);
	this->signal_delete_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onDestroy));

	this->show_all();
}

bool GtkMainWindow::onSecTick()
{
	m_treeview->updateCells();
	return true;
}

void GtkMainWindow::onAddBtnClicked()
{
	Gtk::FileChooserDialog fc("Browse", Gtk::FILE_CHOOSER_ACTION_OPEN);
	fc.set_select_multiple();
	fc.set_transient_for(*this);
	fc.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  	fc.add_button("Select", Gtk::RESPONSE_OK);

  	Glib::RefPtr<Gtk::FileFilter> filter_t = Gtk::FileFilter::create();
	filter_t->set_name("Torrent Files");
	filter_t->add_mime_type("application/x-bittorrent");
	fc.add_filter(filter_t);

	int result = fc.run();

	switch (result) {
		case Gtk::RESPONSE_OK:
			for (auto &f : fc.get_filenames())
			{
				t_ptr t = m_core->addTorrent(f.c_str());
				m_treeview->addCell(t);
			}
		break;
	}
}

void GtkMainWindow::onAddMagnetBtnClicked()
{
	GtkAddMagnetLinkWindow d;
	d.set_transient_for(*this);
	int r = d.run();

	switch (r) {
		case Gtk::RESPONSE_OK:
			t_ptr t = m_core->addTorrent(d.getMagnetURL());
			m_treeview->addCell(t);
		break;
	}
}

bool GtkMainWindow::onDestroy(GdkEventAny *event)
{
	m_core->shutdown();
	return false;
}
