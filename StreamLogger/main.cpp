#pragma once

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>


int main(int argc, const char* argv[])
{
	const int returnCode = Catch::Session().run(argc, argv);

#ifdef _WIN32
	system("pause");
#else
	std::cout << "Press any key to continue ...";
	getch();
#endif

	return returnCode;
}
