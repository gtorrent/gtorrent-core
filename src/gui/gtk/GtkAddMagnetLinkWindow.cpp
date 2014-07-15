#include "GtkAddMagnetLinkWindow.hpp"
#include <gtkmm/button.h>
#include <core/Core.hpp>

GtkAddMagnetLinkWindow::GtkAddMagnetLinkWindow()
{
	this->set_modal(true);
	this->set_default_size(500, 250);
	//this->set_resizable(false);

	//m_header = Gtk::manage(new Gtk::HeaderBar());
	//m_header->set_title("Add Magnet URL");
	//m_header->set_show_close_button(true);

	//this->set_titlebar(*m_header);
	
	m_textview = Gtk::manage(new Gtk::TextView());
	m_textview->set_wrap_mode(Gtk::WRAP_WORD_CHAR);

	this->get_vbox()->pack_start(*m_textview, true, true);

	this->add_button("Add", Gtk::RESPONSE_OK);
	this->add_button("Cancel", Gtk::RESPONSE_CANCEL);
	this->show_all();

	Glib::RefPtr<Gtk::Clipboard> clip = Gtk::Clipboard::get();
	clip->request_text(sigc::mem_fun(*this, &GtkAddMagnetLinkWindow::onClipboardReady));
}

std::string GtkAddMagnetLinkWindow::getMagnetURL()
{
	return m_textview->get_buffer()->get_text();
}

void GtkAddMagnetLinkWindow::onClipboardReady(const Glib::ustring &text)
{
	if (Core::isMagnetLink(text)) {
		Glib::RefPtr<Gtk::TextBuffer> buf = m_textview->get_buffer();
		buf->set_text(text);
	}
}

void GtkAddMagnetLinkWindow::onAddBtnClicked()
{
	close();
}