#pragma once

#include "all.h"

namespace gt
{
	struct Event
	{
		enum Type
		{
			DownloadCompleted
		};

		Type type;
	};
}

