#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <sstream>

namespace
{
	/*
	* Trim leading and trailing space
	*/
	std::string Trim(const std::string& str)
	{
		std::string::const_iterator start = str.begin();
		std::string::const_iterator end = str.end() - 1;

		bool startCorrect = false;
		bool endCorrect = false;

		// Single character check start is space or not
		if (start == end) {
			return std::isspace(*start) ? "" : str;
		}

		while (startCorrect && endCorrect)
		{
			startCorrect = !std::isspace(*start);
			endCorrect = !std::isspace(*end);

			if (!startCorrect) ++start;
			if (!endCorrect) --end;
		}

		return std::string(start, end + 1);
	}

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

	
	/*
	* Return parts of input string split at given delimiter
	* Param 'input' : input string to split
	* Param 'delimiter' : character to split at
	* Param 'trim' : If to trim each parts
	*/
	static std::vector<std::string> Split(
		const std::string& input,
		const char delimiter = ' ',
		const bool trim = false)
	{
		std::vector<std::string> output;
		std::istringstream iss(input);
		std::string token = "";
		while (std::getline(iss, token, delimiter))
		{
			if (!token.empty())
			{
				if (trim)
				{
					token = Trim(token);
					if (!token.empty())
						output.push_back(token);
				}
				else
					output.push_back(token);
			}
		}
		return output;
	}
}