#pragma once

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

	// Signal Responders

	void onAddBtnClicked();
	void onClipboardReady(const Glib::ustring &text);

public:
	GtkAddMagnetLinkWindow();

	// Getters
	
	string getMagnetURL();
};

