#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

int main( int argc, char* argv[] )
{
	boost::log::core::get()->set_filter
	(
		boost::log::trivial::severity >= boost::log::trivial::info
	);
	
	int result = Catch::Session().run( argc, argv );
	return ( result < 0xff ? result : 0xff );
}
