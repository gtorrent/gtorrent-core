#ifndef GTKWINDOW_HPP
#define GTKWINDOW_HPP

#include <gtkmm/window.h>
#include <core/Core.hpp>

namespace gt
{
	/* Is it really necessary to redefine this every time you need it? 
		It's less code to do it this way */
	
	class GtkWindow : public Gtk::Window
	{
	protected:
		core_ptr &m_core;
	public:
		GtkWindow();
	};
};

#endif