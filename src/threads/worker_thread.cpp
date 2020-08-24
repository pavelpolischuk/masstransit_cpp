#include "masstransit_cpp/threads/worker_thread.hpp"

namespace masstransit_cpp
{
	namespace threads
	{
		worker_thread::worker_thread()
			: worker_(*this)
		{
		}

		worker_thread::~worker_thread()
		{
			stop();
		}

		void worker_thread::wait() const
		{
			worker_.attach();
		}
	}
}
