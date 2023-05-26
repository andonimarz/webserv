#include <iostream>

#include "Config.hpp"
#include "Server.hpp"
#include "utils.hpp"

int	main(__attribute__((unused))int argc, __attribute__((unused))char *argv[], __attribute__((unused))char *envp[])
{
	Config config;

	try
	{
		Server server(config);
		server.startServer();
		server.startListen();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}