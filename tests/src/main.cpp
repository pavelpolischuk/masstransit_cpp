#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(const int argc, char* argv[])
{
	const auto result = Catch::Session().run(argc, argv);
	return result < 0xff ? result : 0xff;
}
