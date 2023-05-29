#ifndef WEB_SERVER_HPP
# define WEB_SERVER_HPP

std::vector<std::string> setBody(std::string &path);

enum METHOD
{
	GET,
	POST,
	DELETE
};

#endif