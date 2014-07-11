#include <gtkmm/filechooserdialog.h>
#include "GtkMainWindow.hpp"
#include <Application.hpp>
#include <gtkmm/button.h>

GtkMainWindow::GtkMainWindow() :
	m_core(Application::getSingleton()->getCore())
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

	this->show_all();

	this->signal_delete_event().connect(sigc::mem_fun(*this, &GtkMainWindow::onDestroy));
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
			m_core->getEngine()->addTorrent(fc.get_filename().c_str());
		break;
	}
}

bool GtkMainWindow::onDestroy(GdkEventAny *event)
{
	m_core->shutdown();
	return false;
}