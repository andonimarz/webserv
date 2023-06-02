#include <cstring>
#include <unistd.h>

#include "utils.hpp"
#include "web_server.hpp"

int	printMessage(std::string msg)
{
	std::cout << msg << std::endl;
	return (0);
}

int	printError(std::string error)
{
	std::cerr << error << std::endl;
	return (0);
}

char	**getCharMap(std::map<std::string, std::string> map)
{
	int		size = map.size();
	char**	values = new char*[size + 1];
	
	int	i = 0;
	std::string line;
	for (std::map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); ++it)
	{
		line = it->first + "=" + it->second;
		values[i] = new char[line.size() + 1];
		std::strcpy(values[i], line.c_str());
		i++;
	}
	values[i] = NULL;
	return (values);
}

std::string	readFileDescriptor(int fd)
{
	std::stringstream	ss;
    char				buffer[BUFFER_SIZE];

    while (1)
	{
        int bytesRead = read(fd, buffer, BUFFER_SIZE);
        if (bytesRead <= 0)
            break ;
        ss.write(buffer, bytesRead);
    }

    return (ss.str());
}
