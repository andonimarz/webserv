#ifndef WEB_SERVER_HPP
# define WEB_SERVER_HPP

# define BUFFER_SIZE 1024

# include "Config.hpp"
# include <vector>
# include <string>

class Config;

std::vector<std::string> setBody(std::string &path);

enum METHOD
{
	GET,
	POST,
	DELETE
};

extern Config	config;

#endif