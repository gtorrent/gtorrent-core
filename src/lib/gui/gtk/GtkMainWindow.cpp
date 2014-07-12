#include "GtkTorrentExplorerWindow.hpp"
#include <gtkmm/filechooserdialog.h>
#include "GtkMainWindow.hpp"
#include <Application.hpp>
#include <gtkmm/button.h>
#include <glibmm.h>

GtkMainWindow::GtkMainWindow()
{
	this->set_position(Gtk::WIN_POS_CENTER);
	this->set_default_size(1280, 720);

	header = Gtk::manage(new Gtk::HeaderBar());
	header->set_title("gTorrent");
	header->set_show_close_button(true);

	Gtk::Button *btn = new Gtk::Button();
	btn->set_label("Add");
	btn->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddBtnClicked));
	header->add(*btn);

	this->set_titlebar(*header);

	m_treeview = Gtk::manage(new GtkTorrentTreeView());
	this->add(*m_treeview);

	this->show_all();

	Glib::signal_timeout().connect(sigc::mem_fun(*this, &GtkMainWindow::onSecTick), 10);
	this->signal_delete_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onDestroy));
}

bool GtkMainWindow::onSecTick()
{
	m_treeview->updateCells();
	return true;
}

void GtkMainWindow::onAddBtnClicked()
{
	Gtk::FileChooserDialog fc("Browse", Gtk::FILE_CHOOSER_ACTION_OPEN);
	fc.set_transient_for(*this);
	fc.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  	fc.add_button("Select", Gtk::RESPONSE_OK);

  	Glib::RefPtr<Gtk::FileFilter> filter_t = Gtk::FileFilter::create();
	filter_t->set_name("Torrent Files");
	filter_t->add_mime_type("application/x-bittorrent");
	fc.add_filter(filter_t);

	int result = fc.run();

	switch (result)
	{
		case Gtk::RESPONSE_OK:
			t_ptr t = m_core->getEngine()->addTorrent(fc.get_filename().c_str());
			m_treeview->addCell(t);
		break;
	}
}

bool GtkMainWindow::onDestroy(GdkEventAny *event)
{
	m_core->shutdown();
	return false;
}