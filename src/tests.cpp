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
        if (configs[i].host.empty())
            throw std::runtime_error("Error: host is empty.");
        if (configs[i].server_name.size() == 0)
            throw std::runtime_error("Error: server_name is empty.");
        // Checks if port is in registered ports range
        if (1024 > configs[i].port || configs[i].port > 49151)
            throw std::runtime_error("Error: invalid port number.");
        // Check if there are duplicated ports
        if (std::find(ports.begin(), ports.end(), configs[i].port) != ports.end())
            throw std::runtime_error("Error: duplicated ports.");
        ports.push_back(configs[i].port);
        // Minitest for error_pages
        testErrorPage(configs[i].error_page);
        // Minitests for location{}
        testLocation(configs[i].location);
        /* for(size_t j = 0; j < configs[i].server_name.size(); j++)
        {
            if (std::find(hosts.begin(), hosts.end(), configs[i].server_name[j]) != hosts.end())
                throw std::runtime_error("Error: duplicated hosts.");
            hosts.push_back(configs[i].server_name[j]);
        } */
    }
}

void testErrorPage(std::vector<Config::t_error_page> errors)
{
    size_t errorPageOk = 0;

    if (errors.empty())
        throw std::runtime_error("Error: you need at least one error page.");
    for (size_t i = 0; i < errors.size(); i++)
    {
        // At least should be one error page
        if (errors[i].n_error == 404)
            errorPageOk = 1;
        // Checks if there are duplicated paths or error numbers
        for (size_t j = 0; j < errors.size(); j++)
            if (j != i && (errors[i].n_error == errors[j].n_error || errors[i].path == errors[j].path))
                throw std::runtime_error("Error: duplicated values in error pages.");
    }
    if (errorPageOk == 0)
        throw std::runtime_error("Error: 404 error page not found.");
}

void testLocation(std::vector<Config::t_location> locations)
{
    if (locations.empty())
		throw std::runtime_error("Error: you need at least one location.");
    size_t root = 0;
    for (size_t i = 0; i < locations.size(); i++)
    {
        // Checking there is at least one method
        if (locations[i].method.size() < 1)
            throw std::runtime_error("Error: Missing location method.");
        // Checking if there is more than one '/'
        if (locations[i].path == "/")
            root++;
        // Checking root
        if (locations[i].root.empty())
            throw std::runtime_error("Error: Missing location root.");
        // If autoindex=off you need an upload path
        if (locations[i].upload_enable && locations[i].upload_path.empty())
            throw std::runtime_error("Error: upload enabled but missing path.");
        if (locations[i].upload_enable == false && !locations[i].upload_path.empty())
            throw std::runtime_error("Error: upload path with upload disabled.");
        // If there is index, checks path
        if (!locations[i].index.empty())
        {
            if (locations[i].upload_enable)
                throw std::runtime_error("Error: index and upload_path.");
            if (locations[i].index != "index.html")
                checkPath(joinPaths(locations[i].root, locations[i].index));
        }
        else if (locations[i].upload_enable == false)
            throw std::runtime_error("Error: missing index");
    }
    if (root != 1)
		throw std::runtime_error("Error: you need one and only one root '/' location.");
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
    if (access(path.c_str(), F_OK) != 0)
        throw std::runtime_error("Error: invalid path '" + path + "'.");
}
