#pragma once

using namespace std;

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

