#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <sstream>
# include <map>
# include <vector>
# include <algorithm>
# include <string>

# include "web_server.hpp"

class Request
{
	public:
		Request(std::string const &str);
		~Request();
		void setFullRequest(std::string const &str);
		std::string getFullRequest(void) const;
		std::string getMethod(void) const;
		std::string getRoute(void) const;
		std::string getProtocol(void) const;
		std::string getHost(void) const;
		std::string getConnection(void) const;
		void getInfo(std::string const &str);
		std::string _fileName; //POST
		std::string _fileContent; //POST
	private:
		std::string	_full_request;
		std::string	_method;
		std::string	_route;
		std::string	_protocol;
		std::string _host;
		std::string _connection;
		std::string _boundary; //POST
		/* int			_socket;

		std::map<std::string, std::string>	header;

		std::string	_body; */
};
std::ostream &operator<<( std::ostream &ost, Request const &src );

#endif