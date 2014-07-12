#ifndef GTKWINDOW_HPP
#define GTKWINDOW_HPP

#include <gtkmm/window.h>
#include <core/Core.hpp>

namespace gt
{
	class GtkWindow : public Gtk::Window
	{
	protected:
		core_ptr &m_core;
	public:
		GtkWindow();
	};
};

#endif