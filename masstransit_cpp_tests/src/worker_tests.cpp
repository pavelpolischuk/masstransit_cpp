#include "catch.hpp"

#include <masstransit_cpp/threads/worker_thread.hpp>
#include <masstransit_cpp/threads/thread_pool.hpp>
#include <masstransit_cpp/threads/task_repeat.hpp>

namespace masstransit_cpp_tests
{
	using namespace std;
	using namespace masstransit_cpp::threads;

	TEST_CASE("worker_tests", "[worker]")
	{
		SECTION("worker_do_task_on_other_thread")
		{
			tasks_queue q;
			worker w(q);

			auto thread_id(this_thread::get_id());
			q.enqueue([&thread_id]
			{
				thread_id = this_thread::get_id();
			}).get();
			
			this_thread::sleep_for(chrono::milliseconds(50));
			q.stop();
			
			REQUIRE(thread_id != this_thread::get_id());
		}

		SECTION("worker_thread_do_tasks_on_other_thread")
		{
			worker_thread th;
			auto thread_id_1(this_thread::get_id());
			auto thread_id_2(this_thread::get_id());

			mutex mutex_; mutex_.lock();
			auto task_1 = th.enqueue([&thread_id_1, &mutex_]
			{
				unique_lock<mutex> lock(mutex_);
				thread_id_1 = this_thread::get_id();
			});
			
			auto task_2 = th.enqueue([&thread_id_2, &mutex_]
			{
				unique_lock<mutex> lock(mutex_);
				thread_id_2 = this_thread::get_id();
			});

			this_thread::sleep_for(chrono::milliseconds(50));
			mutex_.unlock();

			task_1.get();
			task_2.get();

			REQUIRE(thread_id_1 != this_thread::get_id());
			REQUIRE(thread_id_2 != this_thread::get_id());
			REQUIRE(thread_id_1 == thread_id_2);
		}

		SECTION("thread_pool_do_tasks_on_other_diff_threads")
		{
			thread_pool th(3);

			auto thread_id_1(this_thread::get_id());
			auto thread_id_2(this_thread::get_id());
			
			mutex mutex_; mutex_.lock();
			auto task_1 = th.enqueue([&thread_id_1, &mutex_]
			{
				unique_lock<mutex> lock(mutex_);
				thread_id_1 = this_thread::get_id();
			});
			
			auto task_2 = th.enqueue([&thread_id_2, &mutex_]
			{
				unique_lock<mutex> lock(mutex_);
				thread_id_2 = this_thread::get_id();
			});

			this_thread::sleep_for(chrono::milliseconds(50));
			mutex_.unlock();

			task_1.get();
			task_2.get();

			REQUIRE(thread_id_1 != this_thread::get_id());
			REQUIRE(thread_id_2 != this_thread::get_id());
			REQUIRE(thread_id_1 != thread_id_2);
		}
		
		SECTION("task_repeate_wait_3_times")
		{
			auto count = 0;
			task_repeat th(chrono::milliseconds(80), [&count]() { ++count; return false; });
			
			this_thread::sleep_for(chrono::milliseconds(200));
			
			th.stop();

			REQUIRE(count == 3);
		}

		SECTION("task_repeate_do_3_times_before_timeout")
		{
			auto count = 0;
			task_repeat th(chrono::seconds(2), [&count]() { return ++count <= 2; });
			
			this_thread::sleep_for(chrono::milliseconds(100));
			
			th.stop();

			REQUIRE(count == 3);
		}
	}
}
