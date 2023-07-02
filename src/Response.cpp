#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "Response.hpp"
#include "web_server.hpp"
#include "utils.hpp"

// ===== Constructor =====
Response::Response(const Config conf, Request const &request) : _request(request)
{
	_config = conf;
	_fullPath = "";
	_absoluteRoute = "";
	_route = "";
	_errorPage = 0;
	return ;
}
// ===== Destructor =====
Response::~Response()
{
	return ;
}

// ===== Getters =====
std::string	Response::_getFullResponse(void)
{
	return (this->_fullResponse);
}

// Check route, handle error pages, set environment variables and call CGI
int	Response::generateResponse(void)
{
	int 		i = checkRoute();

	if (this->_routeExist == false || i >= this->_config.getSizeLocation())
		setErrorPage(404);
	if (_errorPage == 0)
		methodBuild(i);
	if (_errorPage == 0)
	{
		this->_config.exportEnv("REQUEST_METHOD", _request.getMethod());
		this->_config.exportEnv("REQUEST_STATUS", "200");
		this->_config.exportEnv("CONFIG_HOST", this->_config.getHost());
		std::stringstream ss;
    	ss << this->_config.getPort();
		this->_config.exportEnv("CONFIG_PORT", ss.str());
	}
	executeCGI();
	return (0);
}

// Check if request path exists
int	Response::checkRoute(void)
{
	int	i = 0;
	this->_routeExist = false;

	if (_request.getHost() == "")
	{
		setErrorPage(403);
		return (i);
	}
	getRoutes();
	for (i = 0; i < this->_config.getSizeLocation() && this->_routeExist == false; i++)
	{
		if (_absoluteRoute == this->_config.getLocation(i).path)
		{
			this->_routeExist = true;
			break ;
		}
		if (_request.getMethod() != "GET" || this->_config.getLocation(i).path != "/" || _route != "")
			continue ;
		std::ifstream file(this->_config.getLocation(i).root + _request.getRoute());
		if (file.is_open() != 0)
		{
			file.close();
			this->_routeExist = true;
			this->_route = _request.getRoute();
			break ;
		}
	}
	return (i);
}

// Separe route: "/uploads/index.html" -> "/uploads" "/index.html"
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

// Manage request depending on the method
int	Response::methodBuild(int location_index)
{	
	if (this->_config.getLocation(location_index).upload_enable)
		_fullPath = this->_config.getLocation(location_index).upload_path;
	else
		_fullPath = this->_config.getLocation(location_index).root;

	if (_request.getMethod() == "GET" && checkMethodRequest(location_index, GET) == 0)
	{
		if (_request.getRoute() == this->_config.getLocation(location_index).path)
			_fullPath += "/" + this->_config.getLocation(location_index).index;
		else
			_fullPath += "/." + _route;
		std::ifstream file(_fullPath);
		if (!file.is_open())
			return (setErrorPage(404));
		file.close();
	}
	else if (_request.getMethod() == "POST" && checkMethodRequest(location_index, POST) == 0)
		return (buildPost());
	else if (_request.getMethod() == "DELETE" && checkMethodRequest(location_index, DELETE) == 0)
		return (buildDelete());
	else
		setErrorPage(403);

	return (0);
}

// Check that the file does not exist and create it
int	Response::buildPost(void)
{
    mkdir(_fullPath.c_str(), 0777);
	_fullPath += "/" + _request.getFilename();
	std::ifstream file(_fullPath);
	if (file.is_open())
	{
		file.close();
		return (setErrorPage(403));
	}
	this->_config.exportEnv("REQUEST_FILE", _fullPath);
	std::ofstream new_file;
	new_file.open(_fullPath, std::ios::out);
	if (new_file.is_open())
	{
		for (size_t i = 0; i < _request._fileContent.size(); i++)
			new_file << _request._fileContent[i];
		new_file.close();
	}
	else
		return (setErrorPage(500));
	
	return (0);
}

// Check that the file exists
int	Response::buildDelete(void)
{
	_fullPath += _route;
	std::ifstream file(_fullPath);
	if (file.is_open() == 0)
		return (setErrorPage(404));
	file.close();
	this->_config.exportEnv("REQUEST_ROUTE", _fullPath);
	return (0);
}

// Check if the method exists in a location
int	Response::checkMethodRequest(int location_index, int method)
{
	for (int i = 0; i < (int)this->_config.getLocation(location_index).method.size(); i++)
		if (this->_config.getLocation(location_index).method[i] == method)
			return (0);
	return (1);
}

// Change REQUEST_METHOD variable to GET and look for the corresponding error page
int	Response::setErrorPage(int error)
{
	int	i;

	if (_errorPage != 0)
		return (0);
	this->_config.exportEnv("REQUEST_METHOD", "GET");
	_errorPage = error;
	for (i = 0; i < 3; i++)
		if (this->_config.getErrorPage(i).n_error == error)
		{
			_fullPath = this->_config.getErrorPage(i).path;
			std::stringstream ss;
			ss << error;
			this->_config.exportEnv("REQUEST_STATUS", ss.str());
			break ;
		}
	
	return (0);
}

// Execute the CGI program and save the response of the same
int	Response::executeCGI(void)
{
	int		fd[2];
	int		pid;
	char	*cgi_args[] = {(char *)"./cgi/cgi.sh", (char *)_fullPath.c_str(), NULL};

	//this->_config.printEnv();

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
		std::cerr << "Error: CGI failed." << std::endl;
		exit(0);
	}
	else
	{
		close(fd[1]);
		std::cout << "Esperando al hijo, path: " << (char *)_fullPath.c_str() << std::endl;
		waitpid(pid, NULL, 0);
		std::cout << "El hijo ha terminado" << std::endl;
		int fd_response = dup(fd[0]);
		close(fd[0]);
		//std::cout << "readFileDescriptor() ha empezado" << std::endl;
		_fullResponse = readFileDescriptor(fd_response);
		//std::cout << "readFileDescriptor() ha terminado" << std::endl;
	}
	return (0);
}
