#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Request.hpp"

class Response
{
public:
	std::string	_fullResponse;
	std::string	_fullPath;
	Request		_request;
	std::string	_absoluteRoute;
	std::string	_route;
	int			_errorPage;
	Config		_config;
	bool		_routeExist;

public:
	// ===== Constructor =====
	Response(const Config conf, Request const &request);
	// ===== Destructor =====
	~Response();

	// ===== Methods =====
	int	generateResponse(void);

private:
	// ===== Private methods =====
	int	checkMethodRequest(int location_index, int method);
	int	getRoutes(void);
	int	setErrorPage(int error);
	int	methodBuild(int location_index);
	int	executeCGI(void);
	int	buildPost(void);
	int	buildDelete(void);
	int	checkRoute(void);
};

#endif