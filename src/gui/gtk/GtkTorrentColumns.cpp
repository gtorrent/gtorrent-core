#include "GtkTorrentColumns.hpp"

GtkTorrentColumns::GtkTorrentColumns()
{
	add(m_col_name);
	add(m_col_seeders);
	add(m_col_percent);
	add(m_col_percent_text);
}