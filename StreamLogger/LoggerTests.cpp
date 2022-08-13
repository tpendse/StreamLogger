#pragma once

#include <catch2/catch.hpp>

#include "Utilities.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <random>

#include "Logger.h"

using namespace StreamLogger;

constexpr const char* INFO_TEST_MESSAGE = "INFORMATION";
constexpr const char* WARN_TEST_MESSAGE = "WARNING";
constexpr const char* ERROR_TEST_MESSAGE = "ERROR";

#ifdef _WIN32
#define NEW_LINE (const char)'\r\n'
#else
#define NEW_LINE (const char)'\n'
#endif

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

class LogFileCleanup
{
//private:
//	std::vector<std::string> _logFilenames;

public:
	static std::string Get() {
		std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
		std::random_device device;
		std::mt19937 generator(device());
		std::shuffle(str.begin(), str.end(), generator);
		return std::string("TestLog_") + str.substr(0, 10) + ".log";
	}
};

//======================================================================================================

std::unique_ptr<Logger> logger;

std::string LastLogFilename;

void CreateLogger(bool empty = true)
{
	if (empty) {
		logger.reset(new Logger());
	}
	else {
		logger.reset(new Logger(LogFileCleanup::Get()));
	}
	LastLogFilename = logger->Filename();

	// Remove the earlier log file if it already exists
	if (std::filesystem::exists(LastLogFilename))
	{
		for(unsigned short i = 0; i < 5; ++i)
		{
			try {
				if (std::filesystem::remove(LastLogFilename))
					break;
			}
			catch (...) {
				// Wait before trying again
				std::this_thread::sleep_for(std::chrono::milliseconds(750));
			}
		}
	}
}

TEST_CASE("Logger Tests", "[Logger]")
{
	GIVEN("Initializing logger without path")
	{
		CreateLogger();
		std::filesystem::path cwd = std::filesystem::current_path();
		const std::string log_filename = logger->Filename();
		const std::string log_fullpath = JoinPaths(cwd.string(), log_filename);

		WHEN("Logger is constructed")
		{
			THEN("Defaults are correct")
			{
				REQUIRE_FALSE(log_filename.empty());
				REQUIRE(logger->GetEnabled());
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
		CreateLogger();
		std::filesystem::path cwd = std::filesystem::current_path();
		const std::string log_filename = logger->Filename();
		const std::string log_fullpath = JoinPaths(cwd.string(), log_filename);

		/*
		* Tests assume the log message has the format
		* [Timestamp] [INFO/WARN/ERROR] : Message
		*/

		WHEN("Logging info message")
		{
			logger->Info() << INFO_TEST_MESSAGE;
			logger->Flush();

			THEN("Message is logged")
			{
				const std::string contents = GetFileContents(log_fullpath);
				const auto parts = Split(contents, NEW_LINE, true);

				REQUIRE(parts.size() == 4);

				auto msgparts = Split(parts[3]);
				REQUIRE(msgparts.size() == 5);
				REQUIRE(msgparts[4] == INFO_TEST_MESSAGE);
			}
		}

		WHEN("Logging warning message")
		{
			logger->Warn() << WARN_TEST_MESSAGE;
			logger->Flush();

			THEN("Message is logged")
			{
				const std::string contents = GetFileContents(log_fullpath);
				const auto parts = Split(contents, NEW_LINE, true);

				REQUIRE(parts.size() == 4);

				auto msgparts = Split(parts[3]);
				REQUIRE(msgparts.size() == 5);
				REQUIRE(msgparts[4] == WARN_TEST_MESSAGE);
			}
		}

		WHEN("Logging error message")
		{
			logger->Error() << ERROR_TEST_MESSAGE;
			logger->Flush();

			THEN("Message is logged")
			{
				const std::string contents = GetFileContents(log_fullpath);
				const auto parts = Split(contents, NEW_LINE, true);

				REQUIRE(parts.size() == 4);

				auto msgparts = Split(parts[3]);
				REQUIRE(msgparts.size() == 4);
				REQUIRE(msgparts[3] == ERROR_TEST_MESSAGE);
			}
		}
	}

	GIVEN("Disabled logger available")
	{
		// Sleep for some time so that the filestamps don't overlap!
		std::this_thread::sleep_for(std::chrono::milliseconds(750));

		CreateLogger();
		std::filesystem::path cwd = std::filesystem::current_path();
		const std::string log_filename = logger->Filename();
		const std::string log_fullpath = JoinPaths(cwd.string(), log_filename);

		logger->SetEnabled(false);

		WHEN("Logging info message")
		{
			logger->Info() << INFO_TEST_MESSAGE;
			logger->Flush();

			THEN("Message is `not` logged")
			{
				const std::string contents = GetFileContents(log_fullpath);
				INFO("Contents are: " << contents);
				REQUIRE(contents.empty());
			}
		}

		WHEN("Logging warning message")
		{
			logger->Warn() << WARN_TEST_MESSAGE;
			logger->Flush();

			THEN("Message is `not` logged")
			{
				const std::string contents = GetFileContents(log_fullpath);
				INFO("Contents are: " << contents);
				REQUIRE(contents.empty());
			}
		}

		WHEN("Logging error message")
		{
			logger->Error() << ERROR_TEST_MESSAGE;
			logger->Flush();

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