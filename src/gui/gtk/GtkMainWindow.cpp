#include "GtkAddMagnetLinkWindow.hpp"
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/hvseparator.h>
#include "GtkMainWindow.hpp"
#include <Application.hpp>
#include <gtkmm/stock.h>
#include <glibmm.h>

GtkMainWindow::GtkMainWindow() :
	m_core(Application::getSingleton()->getCore())
{
	//TODO:This needs to be refactored
	this->set_position(Gtk::WIN_POS_CENTER);
	this->set_default_size(800, 500);
	m_treeview = Gtk::manage(new GtkTorrentTreeView());
	this->add(*m_treeview);

	Glib::signal_timeout().connect(sigc::mem_fun(*this, &GtkMainWindow::onSecTick), 10);
	this->signal_delete_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onDestroy));

	header = Gtk::manage(new Gtk::HeaderBar());
	header->set_show_close_button(true);
	//TODO: add max/minimise buttons, next to the close button
	//header->set_decoration_layout(
	//		connect_btn,add_torrent_btn,add_link_btn,up_btn,down_btn,pause_btn,
	//		remove_btn:maximise,minimize,close);
	//TODO:Here's a nifty connect button to hang connection settings, rate limiter from.
	Gtk::Button *connect_btn = Gtk::manage(new Gtk::Button());
	connect_btn->set_image_from_icon_name("gtk-directory");
	header->add(*connect_btn);

	Gtk::VSeparator *separator0 = Gtk::manage(new Gtk::VSeparator());
	header->add(*separator0);

	Gtk::Button *add_torrent_btn = Gtk::manage(new Gtk::Button());
	add_torrent_btn->set_image_from_icon_name("gtk-add");
	add_torrent_btn->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddBtnClicked));
	header->add(*add_torrent_btn);

	Gtk::Button *add_link_btn = Gtk::manage(new Gtk::Button());
	add_link_btn->set_image_from_icon_name("edit-paste");
	add_link_btn->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onAddMagnetBtnClicked));
	header->add(*add_link_btn);

	Gtk::VSeparator *separator = Gtk::manage(new Gtk::VSeparator());
	header->add(*separator);

	Gtk::Button *resume_btn = Gtk::manage(new Gtk::Button());
	resume_btn->set_image_from_icon_name("media-playback-start");
	resume_btn->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onResumeBtnClicked));
	header->add(*resume_btn);

	Gtk::Button *pause_btn = Gtk::manage(new Gtk::Button());
	pause_btn->set_image_from_icon_name("media-playback-pause");
	pause_btn->signal_clicked().connect(sigc::mem_fun(*this, &GtkMainWindow::onPauseBtnClicked));
	header->add(*pause_btn);

	Gtk::VSeparator *separator1 = Gtk::manage(new Gtk::VSeparator());
	header->add(*separator1);

	Gtk::Button *up_btn = Gtk::manage(new Gtk::Button());
	up_btn->set_image_from_icon_name("gtk-go-up");
	header->add(*up_btn);

	Gtk::Button *down_btn = Gtk::manage(new Gtk::Button());
	down_btn->set_image_from_icon_name("gtk-go-down");
	header->add(*down_btn);

	Gtk::VSeparator *separator2 = Gtk::manage(new Gtk::VSeparator());
	header->add(*separator2);


	Gtk::Button *remove_btn = Gtk::manage(new Gtk::Button());
	remove_btn->set_image_from_icon_name("gtk-cancel");
	header->add(*remove_btn);

	Gtk::VSeparator *separator3 = Gtk::manage(new Gtk::VSeparator());
	header->add(*separator3);

	Gtk::Button *properties_btn = Gtk::manage(new Gtk::Button());
	properties_btn->set_image_from_icon_name("gtk-properties");
	//TODO:align properties button to right of top bar
	//properties_btn->set_alignment(1.0f,0.0f);

	header->add(*properties_btn);
	this->set_titlebar(*header);
	//status = Gtk::manage(new Gtk::StatusBar());
	//this->set_decorated(FALSE);
	this->set_deletable(FALSE);
	//this->set_hide_titlebar_when_maximized(TRUE);
	this->maximize();
	this->show_all();
	//status = Gtk::manage(new Gtk::StatusBar());
	//this->get_window().set_decoration(64);//WMDecoration.BORDER
	//this.get_window().set_decorations(Gdk.WMDecoration.BORDER);
	//this->set_decorations(FALSE);
	//this->set_decorated(FALSE);
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
	m_treeview->setSelectedPaused(true);
}

void GtkMainWindow::onResumeBtnClicked()
{
	m_treeview->setSelectedPaused(false);
}

void GtkMainWindow::onRemoveBtnClicked()
{
	//get the torrent selected in treeview

	//remove the torrent from treeview
	//torrent.remove();
}
/*
void GtkMainWindow::onPropertiesBtnClicked()
{
	GtkPropertiesWindow d;
	d.set_transient_for(*this);
	int r = d.run();

	switch (r)
	{
	case Gtk::RESPONSE_OK:
		//TODO: Store slected settings to .config file
		break;
	}
}*/

bool GtkMainWindow::onDestroy(GdkEventAny *event)
{
	m_core->shutdown();
	return false;
}
