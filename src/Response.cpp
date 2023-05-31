#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Response.hpp"
#include "web_server.hpp"
#include "utils.hpp"

Response::Response(Request const &request) : _request(request)
{
	_fullPath = "";
	_errorPage = 0;
	return ;
}

Response::~Response()
{
	return ;
}

int	Response::generateResponse(void)
{
	bool	route_exist = false;
	int 	i;

	for (i = 0; i < (int)config.location.size() && route_exist == false; i++)
	{
		if (_request.getRoute() == config.location[i].path)
		{
			route_exist = true;
			break ;
		}
		if (config.location[i].path != "/")
			continue ;
		std::ifstream file(config.location[i].root + _request.getRoute());
		if (file.is_open() != 0)
		{
			file.close();
			route_exist = true;
			break ;
		}
	}	
	
	if (route_exist == false || i >= (int)config.location.size())
		setErrorPage(404);
	if (_errorPage == 0)
		methodBuild(i);
	if (_errorPage == 0)
		config.exportEnv("REQUEST_METHOD", _request.getMethod());
	executeCGI();
	return (0);
}

int	Response::methodBuild(int location_index)
{
	_fullPath = config.location[location_index].root; // /www/./index.html
	if (_request.getMethod() == "GET" && checkMethodRequest(location_index, GET) == 0)
	{
		if (_request.getRoute() == config.location[location_index].path)
			_fullPath += "/" + config.location[location_index].index;
		else
			_fullPath += "/" + _request.getRoute();
		std::ifstream file(_fullPath);
		if (file.is_open() == 0)
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
		if (new_file)
		{
			new_file << _request._fileContent;
			new_file.close();
		}
		else
			return (setErrorPage(500));
	}
	else if (_request.getMethod() == "DELETE" && checkMethodRequest(location_index, DELETE) == 0)
	{
		std::ifstream file(_fullPath);
		if (file.is_open() == 0)
			return (setErrorPage(404));
		file.close();
		if (!std::remove(_fullPath.c_str()))
			return (setErrorPage(500));
	}
	else
		setErrorPage(403);

	return (0);
}

int	Response::checkMethodRequest(int location_index, int method)
{
	for (int i = 0; i < (int)config.location[location_index].method.size(); i++)
		if (config.location[location_index].method[i] == method)
			return (0);
	return (1);
}

int	Response::setErrorPage(int error)
{
	int	i;

	config.exportEnv("REQUEST_METHOD", "GET");
	_errorPage = error;
	for (i = 0; i < 3; i++)
		if (config.error_page[i].n_error == error)
			break ;
	_fullPath = config.error_page[i].path;	
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
		char **env = getCharMap(config.getEnvironment());
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
		char buffer[BUFFER_SIZE];	
		bzero(buffer, BUFFER_SIZE);
		read(fd_response, buffer, BUFFER_SIZE);
		_fullResponse = std::string(buffer);
	}
	return (0);
}
