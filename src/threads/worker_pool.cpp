#include "masstransit_cpp/threads/worker_pool.hpp"

namespace masstransit_cpp
{
	namespace threads
	{
		worker_pool::worker_pool(const size_t size)
			: max_size_(size)
		{
		}

		worker_pool::~worker_pool()
		{
			std::list<std::shared_ptr<worker>> workers;

			{
				std::unique_lock<std::mutex> lock(mutex_);
				for(auto const& w : workers_)
					workers.push_back(w);
				for(auto const& w : busy_workers_)
					workers.push_back(w);
			}

			for(auto const& w : workers)
				w->stop();
			
			for(auto const& w : workers)
				w->attach();

			std::unique_lock<std::mutex> lock(mutex_);
			busy_workers_.clear();
			workers_.clear();
		}

		std::shared_ptr<worker> worker_pool::get_free_worker()
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (!workers_.empty())
				return extract_free();

			if (busy_workers_.size() < max_size_)
				return create();

			condition_.wait(lock, 
				[this] { return !workers_.empty(); });

			return extract_free();
		}

		void worker_pool::return_worker(std::shared_ptr<worker> const& free_worker)
		{
			{
				std::unique_lock<std::mutex> lock(mutex_);
				busy_workers_.remove(free_worker);
				workers_.push_back(free_worker);
			}

			condition_.notify_one();
		}

		std::shared_ptr<worker> worker_pool::create()
		{
			auto w = std::make_shared<worker>([this] (std::shared_ptr<worker> const& free) { return_worker(free); });
			busy_workers_.push_back(w);
			return w;
		}

		std::shared_ptr<worker> worker_pool::extract_free()
		{
			const auto w = workers_.back();
			workers_.pop_back();
			busy_workers_.push_back(w);
			return w;
		}
	}
}
