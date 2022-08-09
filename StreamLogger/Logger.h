#pragma once

#include <memory>
#include <mutex>

namespace StreamLogger
{
	class Logger
	{
	private:
		static std::mutex s_lock;
		bool _enabled;
		std::unique_ptr<std::ostream> _streamPtr;
		std::string _path;

	public:
		Logger(const std::string& path = ""); // By default, write to the execution folder
		virtual ~Logger();

		// Inherited via ILogger
		virtual void SetEnabled(bool enabled);
		virtual bool GetEnabled() const;
		virtual std::ostream& Info() ;
		virtual std::ostream& Warn() ;
		virtual std::ostream& Error();

		virtual std::string Filename() const
		{
			return _path;
		}

		virtual void Flush();

	private:
		//! NOTE Internal methods not thread safe! Make sure you have lock before executing
		std::string GetTimeStampAsFormat(const std::string& format) const;

		//! NOTE Internal methods not thread safe! Make sure you have lock before executing
		std::ostream& Stream();
	};
}