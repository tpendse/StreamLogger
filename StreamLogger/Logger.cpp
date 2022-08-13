#pragma once

#include "Logger.h"
#include "Utilities.h"
#include <fstream>
#include <chrono>
#include <sstream>
#include <filesystem>


class NullBuffer : public std::streambuf
{
public:
	int overflow(int c) { return c; }
};


class NullStream : public std::ostream
{
private:
	NullBuffer m_sb;

public:
	NullStream() : std::ostream(&m_sb) {}
};

using namespace StreamLogger;

//constexpr const char* LOG_FILENAME_FORMAT         = "StreamLogger__%Y%m%d__%H%M%S.log";
constexpr const char* LOG_FILENAME_FORMAT = "StreamLogger.log";
constexpr const char* LOG_HEADER_FORMAT = "StreamLogger Log : Dated %Y-%m-%d %H:%M:%S";
constexpr const char* LOG_ENTRY_TIMESTAMP_FORMAT = "[%H:%M:%S] ";
constexpr const char* LOG_ENTRY_INFO_TAG = "[INFO ] : ";
constexpr const char* LOG_ENTRY_WARN_TAG = "[WARN ] : ";
constexpr const char* LOG_ENTRY_ERROR_TAG = "[ERROR] : ";

std::mutex Logger::s_lock;

/*
* Join the paths with appropriate delimiters
*/
static std::string JoinPaths(const std::string& path1, const std::string& path2)
{
	const std::filesystem::path fsPath1 = path1;
	const std::filesystem::path fsPath2 = path2;
	const std::filesystem::path joined = fsPath1 / fsPath2;
	return joined.string();
}

Logger::Logger(const std::string& path) : _enabled(false), _streamPtr(nullptr)
{
	const std::string logfile = GetTimeStampAsFormat(LOG_FILENAME_FORMAT);
	_path = JoinPaths(path, logfile);

	// By default logger is enabled
	this->SetEnabled(true);
}

Logger::~Logger()
{
	if (_streamPtr && _enabled) {
		auto stream = dynamic_cast<std::ofstream*>(_streamPtr.get());
		stream->close();
	}
	this->Flush();
}

std::ostream& Logger::Info()
{
	std::scoped_lock<std::mutex> lock(s_lock);
	std::ostream& stream = Stream();
	stream << GetTimeStampAsFormat(LOG_ENTRY_TIMESTAMP_FORMAT) << LOG_ENTRY_INFO_TAG;
	return stream;
}

std::ostream& Logger::Warn()
{
	std::scoped_lock<std::mutex> lock(s_lock);
	std::ostream& stream = Stream();
	stream << GetTimeStampAsFormat(LOG_ENTRY_TIMESTAMP_FORMAT) << LOG_ENTRY_WARN_TAG;
	return stream;
}

std::ostream& Logger::Error()
{
	std::scoped_lock<std::mutex> lock(s_lock);
	std::ostream& stream = Stream();
	stream << GetTimeStampAsFormat(LOG_ENTRY_TIMESTAMP_FORMAT) << LOG_ENTRY_ERROR_TAG;
	return stream;
}

std::string Logger::GetTimeStampAsFormat(const std::string& format) const
{
	std::chrono::time_point time = std::chrono::system_clock::now();
	std::time_t tt = std::chrono::system_clock::to_time_t(time);
	std::tm tm;
	localtime_s(&tm, &tt);
	std::stringstream ss;
	ss << std::put_time(&tm, format.c_str());

	return ss.str();
}

std::ostream& Logger::Stream()
{
	if (_streamPtr == nullptr)
	{
		if (_enabled)
		{
			_streamPtr.reset(new std::ofstream(_path.c_str(), std::ios_base::app));

			const std::string divider = "-------------------------------------------------------";
			const std::string message = GetTimeStampAsFormat(LOG_HEADER_FORMAT);
			*_streamPtr << std::endl << divider << std::endl << message << std::endl << divider << std::endl << std::endl;
		}
		else
		{
			_streamPtr.reset(new NullStream);
		}
	}

	return *_streamPtr;
}

void Logger::SetEnabled(bool enabled)
{
	std::scoped_lock<std::mutex> lock(s_lock);
	if (_enabled != enabled)
	{
		_enabled = enabled;

		_streamPtr.reset(nullptr);
	}
}

bool Logger::GetEnabled() const
{
	return _enabled;
}

void Logger::Flush()
{
	std::scoped_lock<std::mutex> lock(s_lock);
	if (_streamPtr != nullptr)
	{
		_streamPtr->flush();
	}
}
