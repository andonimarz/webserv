#include <iostream>

#include "Config.hpp"
#include "Server.hpp"
#include "utils.hpp"

int	main(int argc, char *argv[], __attribute__((unused))char *envp[])
{
	if (argc > 2)
	{
		std::cout << "Error: bad args" << std::endl;
		return 0;
	}
	Config config;

	if (argc == 2)
	{
		std::string str(argv[1]);
		config.setBody(str);
		//std::cout << "BODY = " << config.getBody() << std::endl;
	}

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