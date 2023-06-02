#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <map>

char	**getCharMap(std::map<std::string, std::string> map);
int		printMessage(std::string msg);
int		printError(std::string error);
std::string	readFileDescriptor(int fd);

#endif