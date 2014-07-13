#include <Application.hpp>
#include "GtkWindow.hpp"

gt::GtkWindow::GtkWindow() :
	m_core(Application::getSingleton()->getCore())
{
}