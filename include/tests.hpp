#ifndef TESTS_HPP
# define TESTS_HPP

# include <vector>
# include "Config.hpp"

void testConfigs(std::vector<Config> configs);
void testLocation(std::vector<Config::t_location> locations);
void testErrorPage(std::vector<Config::t_error_page> errors);
void checkPath(const std::string &path);
std::string joinPaths(std::string root, std::string path);

#endif