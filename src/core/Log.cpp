#include "Log.hpp"
#include <cstdio>

void gt::Log::Debug(const char *fmt, ...)
{
	va_list args;

	printf("[TIME]: ");
	vprintf(fmt, args);
}
