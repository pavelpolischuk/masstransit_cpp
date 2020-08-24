#pragma once

#include <masstransit_cpp/threads/worker.hpp>

#include <list>

namespace masstransit_cpp
{
	namespace threads
	{
		class MASSTRANSIT_CPP_API worker_pool
		{
		public:
			explicit worker_pool(size_t size);
			~worker_pool();

			std::shared_ptr<worker> get_free_worker();
			void return_worker(std::shared_ptr<worker> const& free_worker);
			
		private:
			mutable std::mutex mutex_;
			mutable std::condition_variable condition_;
			
			std::list<std::shared_ptr<worker>> workers_;
			std::list<std::shared_ptr<worker>> busy_workers_;
			size_t max_size_;

			std::shared_ptr<worker> create();
			std::shared_ptr<worker> extract_free();
		};
	}
}
