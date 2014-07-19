#include "GtkAddMagnetLinkWindow.hpp"
#include <gtkmm/filechooserdialog.h>
#include "GtkMainWindow.hpp"
#include <Application.hpp>
#include <gtkmm/button.h>
#include <gtkmm/hvseparator.h>
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
	add_torrent_btn->set_image_from_icon_name("gtk-add");
	add_torrent_btn->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddBtnClicked));
	header->add(*add_torrent_btn);

	Gtk::Button *add_link_btn = Gtk::manage(new Gtk::Button());
	add_link_btn->set_image_from_icon_name("gtk-paste");
	add_link_btn->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddMagnetBtnClicked));
	header->add(*add_link_btn);

	Gtk::VSeparator *separator = Gtk::manage(new Gtk::VSeparator());
	header->add(*separator);

	Gtk::Button *pause_btn = Gtk::manage(new Gtk::Button());
	pause_btn->set_image_from_icon_name("gtk-media-pause");
	header->add(*pause_btn);

	Gtk::Button *remove_btn = Gtk::manage(new Gtk::Button());
	remove_btn->set_image_from_icon_name("gtk-cancel");
	header->add(*remove_btn);

	this->set_titlebar(*header);

	Gtk::Button *properties_btn = Gtk::manage(new Gtk::Button());
	properties_btn->set_image_from_icon_name("gtk-properties");
	header->add(*properties_btn);

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
	Gtk::FileChooserDialog fc("Browse for torrent file", Gtk::FILE_CHOOSER_ACTION_OPEN);
	fc.set_default_size(256, 256);
	//fc.set_window_position(0, 128);
	fc.set_select_multiple();
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
		for (auto &f : fc.get_filenames())
		{
			shared_ptr<Torrent> t = m_core->addTorrent(f.c_str());
			if (t)//Checks if t is not null
				m_treeview->addCell(t);
			//TODO Add error dialogue if torrent add is unsuccessful
		}
		break;
	}
}

void GtkMainWindow::onAddMagnetBtnClicked()
{
	GtkAddMagnetLinkWindow d;
	d.set_transient_for(*this);
	int r = d.run();

	switch (r)
	{
	case Gtk::RESPONSE_OK:
		shared_ptr<Torrent> t = m_core->addTorrent(d.getMagnetURL());
		if (t)//Checks if t is not null
			m_treeview->addCell(t);
		//TODO Add error dialogue if torrent add is unsuccessful
		break;
	}
}

void GtkMainWindow::onPauseBtnClicked()
{
	//TODO: handle pausing torrents
}

void GtkMainWindow::onRemoveBtnClicked()
{
	//TODO: handle removing torrents
}

void GtkMainWindow::onPropertiesBtnClicked()
{
	//TODO: handle properties button click
}

bool GtkMainWindow::onDestroy(GdkEventAny *event)
{
	m_core->shutdown();
	return false;
}
