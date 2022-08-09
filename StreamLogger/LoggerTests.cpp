#pragma once

#include <catch2/catch.hpp>

#include "Utilities.h"
#include <filesystem>
#include <fstream>

#include "Logger.h"

using namespace StreamLogger;

constexpr const char* INFO_TEST_MESSAGE = "INFORMATION";
constexpr const char* WARN_TEST_MESSAGE = "WARNING";
constexpr const char* ERROR_TEST_MESSAGE = "ERROR";

//======================================================================================================

std::string GetFileContents(const std::string& filename)
{
	try
	{
		std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

		const unsigned int fileSize = (unsigned int) std::filesystem::file_size(filename);
		ifs.seekg(0, std::ios::beg);

		std::vector<char> bytes(fileSize);
		ifs.read(bytes.data(), fileSize);

		return std::string(bytes.data(), fileSize);
	}
	catch (...)
	{
		return "";
	}
}

//======================================================================================================

TEST_CASE("Logger Tests", "[Logger]")
{
	GIVEN("Initializing logger")
	{
		Logger logger;
		std::filesystem::path cwd = std::filesystem::current_path();
		const std::string log_filename = logger.Filename();
		const std::string log_fullpath = JoinPaths(cwd.string(), log_filename);

		WHEN("Logger is constructed")
		{
			THEN("Defaults are correct")
			{
				REQUIRE_FALSE(log_filename.empty());
				REQUIRE(logger.GetEnabled());
			}
		}

		WHEN("Nothing is logged")
		{
			THEN("Log file not created")
			{
				const std::filesystem::path path(log_fullpath);
				REQUIRE_FALSE(std::filesystem::exists(path));
			}
		}
	}

	GIVEN("Enabled logger available")
	{
		Logger logger;
		std::filesystem::path cwd = std::filesystem::current_path();
		const std::string log_filename = logger.Filename();
		const std::string log_fullpath = JoinPaths(cwd.string(), log_filename);

		/*
		* Tests assume the log message has the format
		* [Timestamp] [INFO/WARN/ERROR] : Message
		*/

		WHEN("Logging info message")
		{
			logger.Info() << INFO_TEST_MESSAGE;
			logger.Flush();

			THEN("Message is logged")
			{
				const std::string contents = GetFileContents(log_fullpath);
				const auto parts = Split(contents);

				REQUIRE(parts.size() == 4);
				REQUIRE(parts[3] == INFO_TEST_MESSAGE);
			}
		}

		WHEN("Logging warning message")
		{
			logger.Warn() << WARN_TEST_MESSAGE;
			logger.Flush();

			THEN("Message is logged")
			{
				const std::string contents = GetFileContents(log_fullpath);
				const auto parts = Split(contents);

				REQUIRE(parts.size() == 4);
				REQUIRE(parts[3] == WARN_TEST_MESSAGE);
			}
		}

		WHEN("Logging error message")
		{
			logger.Error() << ERROR_TEST_MESSAGE;
			logger.Flush();

			THEN("Message is logged")
			{
				const std::string contents = GetFileContents(log_fullpath);
				const auto parts = Split(contents);

				REQUIRE(parts.size() == 4);
				REQUIRE(parts[3] == ERROR_TEST_MESSAGE);
			}
		}
	}

	GIVEN("Disabled logger available")
	{
		// Sleep for some time so that the filestamps don't overlap!
		std::this_thread::sleep_for(std::chrono::milliseconds(750));

		Logger logger;
		std::filesystem::path cwd = std::filesystem::current_path();
		const std::string log_filename = logger.Filename();
		const std::string log_fullpath = JoinPaths(cwd.string(), log_filename);

		logger.SetEnabled(false);

		WHEN("Logging info message")
		{
			logger.Info() << INFO_TEST_MESSAGE;
			logger.Flush();

			THEN("Message is `not` logged")
			{
				const std::string contents = GetFileContents(log_fullpath);
				INFO("Contents are: " << contents);
				REQUIRE(contents.empty());
			}
		}

		WHEN("Logging warning message")
		{
			logger.Warn() << WARN_TEST_MESSAGE;
			logger.Flush();

			THEN("Message is `not` logged")
			{
				const std::string contents = GetFileContents(log_fullpath);
				INFO("Contents are: " << contents);
				REQUIRE(contents.empty());
			}
		}

		WHEN("Logging error message")
		{
			logger.Error() << ERROR_TEST_MESSAGE;
			logger.Flush();

			THEN("Message is `not` logged")
			{
				const std::string contents = GetFileContents(log_fullpath);
				INFO("Contents are: " << contents);
				REQUIRE(contents.empty());
			}
		}
	}
}

//======================================================================================================