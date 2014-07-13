#include "GtkAddMagnetLinkWindow.hpp"
#include <gtkmm/button.h>

GtkAddMagnetLinkWindow::GtkAddMagnetLinkWindow()
{
	this->set_position(Gtk::WIN_POS_CENTER);
	this->set_default_size(600, 350);

	m_header = Gtk::manage(new Gtk::HeaderBar());
	m_header->set_title("Add Magnet URL");
	m_header->set_show_close_button(true);

	this->set_titlebar(*m_header);
	
	m_textview = Gtk::manage(new Gtk::TextView());
	m_textview->set_wrap_mode(Gtk::WRAP_WORD_CHAR);

	this->get_vbox()->pack_start(*m_textview, true, true);

	this->add_button("Add", Gtk::RESPONSE_OK);
	this->add_button("Cancel", Gtk::RESPONSE_CANCEL);
	this->show_all();
}

std::string GtkAddMagnetLinkWindow::getMagnetURL()
{
	return m_textview->get_buffer()->get_text();
}

void GtkAddMagnetLinkWindow::onAddBtnClicked()
{
	close();
}