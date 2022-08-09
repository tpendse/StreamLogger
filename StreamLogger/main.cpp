#pragma once

#include <iostream>
#include "Logger.h"

int main()
{
	StreamLogger::Logger logger;

	logger.Info() << "This is a info" << std::endl;
	logger.Warn() << "This is a warning" << std::endl;
	logger.Error() << "This is an error" << std::endl;

	std::cout << "Logger enabled? " << logger.GetEnabled() << std::endl;
	std::cout << "Disabling logger ..." << std::endl;

	logger.SetEnabled(false);

	logger.Info() << "DISABLED This is a info" << std::endl;
	logger.Warn() << "DISABLED This is a warning" << std::endl;
	logger.Error() << "DISABLED This is an error" << std::endl;
}