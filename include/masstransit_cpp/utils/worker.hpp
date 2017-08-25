#pragma once
#include <masstransit_cpp/utils/tasks_queue.hpp>

namespace masstransit_cpp
{
    class MASSTRANSIT_CPP_EXPORT worker
    {
    public:
        explicit worker(tasks_queue & queue);
		~worker();

    protected:
        std::thread thread_;
    };
}