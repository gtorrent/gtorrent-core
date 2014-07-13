#ifndef GTKADDMAGNETLINKWINDOW_HPP
#define GTKADDMAGNETLINKWINDOW_HPP

#include <gtkmm/headerbar.h>
#include <gtkmm/textview.h>
#include <gtkmm/dialog.h>
#include <gtkmm/main.h>
#include <gtkmm/box.h>

class GtkAddMagnetLinkWindow : public Gtk::Dialog
{
private:
	Gtk::HeaderBar *m_header;
	Gtk::TextView *m_textview; 

	/* Signal Responders */

	void onAddBtnClicked();

public:
	GtkAddMagnetLinkWindow();

	/* Getters */
	std::string getMagnetURL();
};

#endif