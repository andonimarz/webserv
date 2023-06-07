#ifndef UTILS_CONFIG_HPP
# define UTILS_CONFIG_HPP

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <vector>

std::vector<std::string> setBody(std::string &path);
std::vector<std::string> extractServerBlocks(const std::string &configContent);
bool isValidIPAddress(const std::string &ipAddress);

#endif