#pragma once

#include <iostream>
#include "Logger.h"

int main()
{
	StreamLogger::Logger logger;

	logger.Info() << "This is a info" << std::endl;

	logger.Warn() << "This is a warning" << std::endl;

	logger.Error() << "This is an error" << std::endl;
}