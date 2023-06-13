#include "tests.hpp"
#include <unistd.h>

void testConfigs(std::vector<Config> configs)
{
    std::vector<int> ports;
    std::vector<std::string> hosts;
    if (configs.empty())
		throw std::runtime_error("Error: invalid config file path.");
    for (size_t i = 0; i < configs.size(); i++)
    {
        std::cout << "Empezamos\n";
        // Minitests for location{}
        testLocation(configs[i].location);
        // Checks if port is in registered ports range
        if (1024 > configs[i].port || configs[i].port > 49151)
            throw std::runtime_error("Error: invalid port number.");
        // Check if there are duplicated ports
        if (std::find(ports.begin(), ports.end(), configs[i].port) != ports.end())
            throw std::runtime_error("Error: duplicated ports.");
        ports.push_back(configs[i].port);
        for(size_t j = 0; j < configs[i].server_name.size(); j++)
        {
            if (std::find(hosts.begin(), hosts.end(), configs[i].server_name[j]) != hosts.end())
                throw std::runtime_error("Error: duplicated hosts.");
            hosts.push_back(configs[i].server_name[j]);
        }
    }
    //for(size_t i = 0; i < hosts.size(); i++)
    //    std::cout << "!!!!!!!!! " << hosts[i] << std::endl;
}

void testLocation(std::vector<Config::t_location> locations)
{
    if (locations.empty())
		throw std::runtime_error("Error: you need at least one location.");
    size_t root = 0;
    for (size_t i = 0; i < locations.size(); i++)
    {
        size_t paths = 0;
        // Checking there is at least one method
        if (locations[i].method.size() < 1)
            throw std::runtime_error("Error: Missing method.");
        // Checking if there is more than one '/'
        if (locations[i].path == "/")
            root++;
        // If there is index, checks path and counts it
        if (!locations[i].index.empty())
        {
            checkPath(joinPaths(locations[i].root, locations[i].index));
            paths++;
        }
        // If there is upload path, counts it
        if (!locations[i].upload_path.empty())
        {
            // Don't check path because we are creating it
            // checkPath(joinPaths(locations[i].root, locations[i].upload_path));
            paths++;
        }
        // There can only be one path so if there are more or less throw exception
        if (paths != 1)
            throw std::runtime_error("Error: you need one and only one index or upload path");
    }
    if (root != 1)
		throw std::runtime_error("Error: you need one and only one root '/' location.");
    std::cout << "Locations / = " << root << std::endl;
}

std::string joinPaths(std::string root, std::string path)
{
    std::string result = "";
    // Verificar si alguno está vacío
    if (path.empty())
        return root;
    if (root.empty())
        return path;
    // Verificar si el path es absoluto
    if (path[0] == '/')
        path = path.substr(1);
    // Verificar si el path comienza con './'
    if (path.substr(0, 2) == "./")
        path = path.substr(2);
    // Verificar si el path contiene el root
    if (path.substr(0, root.size()) == root)
        return path;
    // Verificar si el root termina con '/'
    if (!root.empty() && root.back() != '/')
        return root + '/' + path;
    return root + path;
}

void checkPath(const std::string &path)
{
    //std::cout << "DING!" << path << std::endl;
    if (access(path.c_str(), F_OK) != 0)
        throw std::runtime_error("Error: invalid path '" + path + "'.");
}
