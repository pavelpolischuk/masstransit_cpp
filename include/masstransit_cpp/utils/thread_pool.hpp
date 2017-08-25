#pragma once

#include <masstransit_cpp/utils/worker.hpp>

#include <list>

namespace masstransit_cpp
{
    class MASSTRANSIT_CPP_EXPORT thread_pool : public tasks_queue
    {
    public:
        explicit thread_pool(size_t pool_size);

		~thread_pool();

    private:
        std::list<worker> workers_;
    };
}