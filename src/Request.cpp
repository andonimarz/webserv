#include "Request.hpp"

Request::Request(std::string const &str)
{
    //std::cout << "[PRUEBAS_ANDONI] Generating request" << std::endl;
    setFullRequest(str);
    getInfo(str);
}

Request::~Request()
{
    //std::cout << "[PRUEBAS_ANDONI] Destroying request" << std::endl;
}

void Request::setFullRequest(std::string const &str)
{
    this->_full_request = str;
}

std::string Request::getFullRequest(void) const
{
    return this->_full_request;
}

std::string Request::getMethod(void) const
{
    return this->_method;
}

std::string Request::getRoute(void) const
{
    return this->_route;
}

std::string Request::getProtocol(void) const
{
    return this->_protocol;
}

std::string Request::getHost(void) const
{
    return this->_host;
}

std::string Request::getConnection(void) const
{
    return this->_connection;
}

void Request::getInfo(std::string const &str)
{
    std::vector<std::string> lines;
    std::vector<std::string> words;
    std::stringstream ss(str);
    std::string line;
    while(std::getline(ss, line))
        lines.push_back(line);
    for(std::vector<std::string>::size_type i = 0; i < lines.size(); i++)
    {
        if(i == 0)
        {
            //GET / HTTP/1.1
            std::istringstream iss(lines[0]); 
            std::string word;
            while(iss >> word)
                words.push_back(word);
        }
        else
        {
            std::string line = lines[i];
            std::string::size_type pos = line.find_first_of(" \t");
            if (pos != std::string::npos) 
            {
                std::string firstWord = line.substr(0, pos);
                if (firstWord == "Host:")
                    this->_host = line.substr(pos + 1);
                if (firstWord == "Connection:")
                    std::cout << this->_connection << std::endl;
            }
        }
    }
    

    this->_method = words[0];
    this->_route = words[1];
    this->_protocol = words[2];
}

std::ostream & operator<<(std::ostream &ost, const Request &src)
{
    ost << "-> METHOD: " << src.getMethod() << std::endl \
        << "-> ROUTE: " << src.getRoute() << std::endl \
        << "-> PROTOCOL: " << src.getProtocol() << std::endl \
        << "-> CONNECTION: " << src.getConnection() << std::endl \
        << "-> HOST: " << src.getHost() << std::endl;
    return ost;
}

    //Falta connection. hay que gestionarlo por si es "close"
    //Habría que cerrar del tirón en cuanto se mande la respuesta.
    //Si es keep-alive hay que mantenerlo encendido.