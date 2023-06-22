#include <iostream>
#include <vector>

#include "Config.hpp"
#include "Server.hpp"
#include "Route.hpp"
#include "utils.hpp"
#include "utils_config.hpp"
#include "tests.hpp"

#include "web_server.hpp"

int	main(int argc, char **argv, char **env)
{
	if (argc > 2)
	{
		std::cout << "Error: bad args" << std::endl;
		return 0;
	}
    Config config;
	config.setEnv(env);
	try
	{
		if (argc == 2)
		{
			std::cout << "Using your cfg file\n";
			std::vector<Config> configs;
			std::string str(argv[1]);
			std::vector<std::string> servers = setBody(str);
			std::vector<std::string>::const_iterator it;
			for (it = servers.begin(); it != servers.end(); ++it)
			{
				const std::string &element = *it;
				//std::cout << "Element: " << element << std::endl;
				Config tmp("");
				tmp.setEnv(env);
				tmp.fillFields(element);
				// tmp.printConf();
				configs.push_back(tmp);
			}
			// De momento solo usamos el primer server
			testConfigs(configs);
			Route route(configs);
			route.startListen();
		} else {
			Route route(config);
			route.startListen();
		}
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
