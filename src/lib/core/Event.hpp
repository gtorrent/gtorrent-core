#ifndef GTEVENT_HPP
#define GTEVENT_HPP
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
#endif