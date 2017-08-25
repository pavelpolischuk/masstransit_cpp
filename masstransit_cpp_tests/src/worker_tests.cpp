#include "catch.hpp"

#include <masstransit_cpp/utils/worker_thread.hpp>
#include <masstransit_cpp/utils/thread_pool.hpp>

namespace masstransit_cpp_tests
{
	using namespace std;
	using namespace masstransit_cpp;

	
	void work(thread::id & thread_id)
	{
		thread_id = this_thread::get_id();
	}

	TEST_CASE("worker_tests", "[worker]")
	{
		SECTION("worker_do_task_on_other_thread")
		{
			tasks_queue q;
			auto w = make_shared<worker>(q);

			auto thread_id(this_thread::get_id());
			q.enqueue([&thread_id]
			{
				thread_id = this_thread::get_id();
			});

			this_thread::sleep_for(chrono::milliseconds(40));
			q.stop();
			
			w.reset();

			REQUIRE(thread_id != this_thread::get_id());
		}

		SECTION("thread_pull_do_tasks_on_other_thread")
		{
			auto th = make_shared<worker_thread>();

			auto thread_id_1(this_thread::get_id());
			auto thread_id_2(this_thread::get_id());
			th->enqueue([&thread_id_1]
			{
				thread_id_1 = this_thread::get_id();
			});
			
			th->enqueue([&thread_id_2]
			{
				thread_id_2 = this_thread::get_id();
			});

			th.reset();

			REQUIRE(thread_id_1 != this_thread::get_id());
			REQUIRE(thread_id_2 != this_thread::get_id());
			REQUIRE(thread_id_1 == thread_id_2);
		}

		SECTION("thread_pool_do_tasks_on_other_diff_threads")
		{
			auto th = make_shared<thread_pool>(3);

			auto thread_id_1(this_thread::get_id());
			auto thread_id_2(this_thread::get_id());
			th->enqueue([&thread_id_1]
			{
				thread_id_1 = this_thread::get_id();
			});
			
			th->enqueue([&thread_id_2]
			{
				thread_id_2 = this_thread::get_id();
			});

			th.reset();

			REQUIRE(thread_id_1 != this_thread::get_id());
			REQUIRE(thread_id_2 != this_thread::get_id());
			REQUIRE(thread_id_1 != thread_id_2);
		}
	}
}