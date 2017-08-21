#include <gtest/gtest.h>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

int main(int argc, char ** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	
	boost::log::core::get()->set_filter
    (
        boost::log::trivial::severity >= boost::log::trivial::info
    );

	return RUN_ALL_TESTS();
}