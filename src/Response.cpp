#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Response.hpp"
#include "web_server.hpp"
#include "utils.hpp"

Response::Response(const Config conf, Request const &request) : _request(request)
{
	_config = conf;
	_fullPath = "";
	_absoluteRoute = "";
	_route = "";
	_errorPage = 0;
	return ;
}

Response::~Response()
{
	return ;
}

int	Response::generateResponse(void)
{
	bool		route_exist = false;
	int 		i;

	getRoutes();

	for (i = 0; i < (int)this->_config.location.size() && route_exist == false; i++)
	{
		if (_absoluteRoute == this->_config.location[i].path)
		{
			route_exist = true;
			break ;
		}
		if (_request.getMethod() != "GET" || this->_config.location[i].path != "/" || _route != "")
			continue ;
		std::ifstream file(this->_config.location[i].root + _request.getRoute());
		if (file.is_open() != 0)
		{
			file.close();
			route_exist = true;
			this->_route = _request.getRoute();
			break ;
		}
	}

	if (route_exist == false || i >= (int)this->_config.location.size())
		setErrorPage(404);
	if (_errorPage == 0)
		methodBuild(i);
	if (_errorPage == 0)
	{
		this->_config.exportEnv("REQUEST_METHOD", _request.getMethod());
		this->_config.exportEnv("REQUEST_STATUS", "200");
		this->_config.exportEnv("CONFIG_HOST", this->_config.host);
		std::stringstream ss;
    	ss << this->_config.port;
		this->_config.exportEnv("CONFIG_PORT", ss.str());
	}
	executeCGI();
	return (0);
}

// "/uploads/index.html" -> "/uploads" "/index.html"
int	Response::getRoutes(void)
{
	size_t i = this->_request.getRoute().find('/', 1);
	if (i != std::string::npos)
	{
		this->_absoluteRoute = this->_request.getRoute().substr(0, i);
		this->_route = this->_request.getRoute().substr(i, this->_request.getRoute().size());
	}
	else
		this->_absoluteRoute = this->_request.getRoute();
	return (0);
}

int	Response::methodBuild(int location_index)
{
	_fullPath = this->_config.location[location_index].root + this->_config.location[location_index].upload_path;
	if (_request.getMethod() == "GET" && checkMethodRequest(location_index, GET) == 0)
	{
		if (_request.getRoute() == this->_config.location[location_index].path)
			_fullPath += "/" + this->_config.location[location_index].index;
		else
			_fullPath += "/." + _route;
		std::ifstream file(_fullPath);
		if (!file.is_open())
			return (setErrorPage(404));
		file.close();
	}
	else if (_request.getMethod() == "POST" && checkMethodRequest(location_index, POST) == 0)
	{
		_fullPath += "/" + _request._fileName;
		std::ifstream file(_fullPath);
		if (file.is_open())
		{
			file.close();
			return (setErrorPage(403));
		}
		std::ofstream new_file;
		new_file.open(_fullPath, std::ios::out);
		if (new_file.is_open())
		{
			new_file << _request._fileContent;
			new_file.close();
		}
		else
			return (setErrorPage(500));
	}
	else if (_request.getMethod() == "DELETE" && checkMethodRequest(location_index, DELETE) == 0)
	{
		_fullPath += _route;
		std::ifstream file(_fullPath);
		if (file.is_open() == 0)
			return (setErrorPage(404));
		file.close();
		this->_config.exportEnv("REQUEST_ROUTE", _fullPath);
	}
	else
		setErrorPage(403);

	return (0);
}

int	Response::checkMethodRequest(int location_index, int method)
{
	for (int i = 0; i < (int)this->_config.location[location_index].method.size(); i++)
		if (this->_config.location[location_index].method[i] == method)
			return (0);
	return (1);
}

int	Response::setErrorPage(int error)
{
	int	i;

	this->_config.exportEnv("REQUEST_METHOD", "GET");
	_errorPage = error;
	for (i = 0; i < 3; i++)
		if (this->_config.error_page[i].n_error == error)
		{
			_fullPath = this->_config.error_page[i].path;
			std::stringstream ss;
			ss << error;
			this->_config.exportEnv("REQUEST_STATUS", ss.str());
			break ;
		}
	
	return (0);
}

int	Response::executeCGI(void)
{
	int		fd[2];
	int		pid;
	char	*cgi_args[] = {(char *)"./cgi/cgi.sh", (char *)_fullPath.c_str(), NULL};

	if (pipe(fd) == -1)
		return (1);
	pid = fork();
	if (pid == -1)
		return (1);
	else if (pid == 0)
	{
		close(fd[0]);
		char **env = getCharMap(this->_config.getEnvironment());
		dup2(fd[1], 1);
		close(fd[1]);
		execve(*cgi_args, cgi_args, env);
	}
	else
	{
		close(fd[1]);
		waitpid(pid, NULL, 0);
		int fd_response = dup(fd[0]);
		close(fd[0]);
		_fullResponse = readFileDescriptor(fd_response);
	}
	return (0);
}
