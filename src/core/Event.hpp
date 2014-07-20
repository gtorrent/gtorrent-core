#pragma once

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

