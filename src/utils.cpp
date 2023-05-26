#include "utils.hpp"

int	printMessage(std::string msg)
{
	std::cout << msg << std::endl;
	return (0);
}

int	printError(std::string error)
{
	std::cerr << error << std::endl;
	return (0);
}
