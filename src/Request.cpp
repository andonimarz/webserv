#include "Request.hpp"

Request::Request(std::vector<char> buf)
{
    //std::cout << "[PRUEBAS_ANDONI] Generating request" << std::endl;
    this->_method = "";
    this->_route = "";
    this->_protocol = "";
    this->_host = "";
    this->_connection = "";
    this->_boundary = "";
    this->_fileName = "";
    this->_full_request = buf;
    this->_contentLength = 0;
    getInfo();
    /* std::cout << "Request: " << std::endl;
    for (size_t i = 0; i < buf.size(); i++)
        std::cout << buf[i]; */
}

Request::~Request() {
    //std::cout << "[PRUEBAS_ANDONI] Destroying request" << std::endl;
}

void Request::setFullRequest(const std::vector<char> &src) {
    this->_full_request = src;
}

std::vector<char> Request::getFullRequest(void) const {
    return this->_full_request;
}

std::string Request::getMethod(void) const {
    return this->_method;
}

std::string Request::getRoute(void) const {
    return this->_route;
}

std::string Request::getProtocol(void) const {
    return this->_protocol;
}

std::string Request::getHost(void) const {
    return this->_host;
}

std::string Request::getConnection(void) const {
    return this->_connection;
}

std::string Request::getFilename(void) const {
    return this->_fileName;
}

std::vector<char> Request::getFileContent(void) const {
    return this->_fileContent;
}

void Request::getInfo(void)
{
    std::string str(this->_full_request.begin(), this->_full_request.end());
    std::vector<std::string> lines;
    std::vector<std::string> words;
    std::stringstream ss(str);
    std::string line;
    while (std::getline(ss, line))
        lines.push_back(line);
    for (std::vector<std::string>::size_type i = 0; i < lines.size(); i++)
    {
        if (i == 0)
        {
            //GET / HTTP/1.1
            std::istringstream iss(lines[0]); 
            std::string word;
            while(iss >> word)
                words.push_back(word);
            this->_method = words[0];
            this->_route = words[1];
            this->_protocol = words[2];
        }
        else
        {
            std::string line = lines[i];
            std::string::size_type pos = line.find_first_of(" \t");
            if (pos != std::string::npos) 
            {
                std::string firstWord = line.substr(0, pos);
                // Saving host
                if (firstWord == "Host:")
                    this->_host = line.substr(pos + 1);
                // Saving connection
                if (firstWord == "Connection:")
                    this->_connection = line.substr(pos + 1);
                if(firstWord == "Content-Length:")
                {
                    std::istringstream iss(line.substr(pos + 1));
                    iss >> this->_contentLength;
                    //std::cout << "-> Content-Length: " << this->_contentLength << std::endl;
                }
            }
            pos = line.find("boundary=");
            if (pos != std::string::npos)
            {
                this->_boundary = line.substr(pos + 9);
                //std::cout << "-> Boundary: " << this->_boundary << std::endl;
            }
            pos = line.find("filename=");
            if (pos != std::string::npos)
            {
                // Saving filename
                std::string st = line.substr(pos + 9);
                st.erase(std::remove(st.begin(), st.end(), '\"'), st.end());
                st.erase(std::remove(st.begin(), st.end(), '\r'), st.end());
                this->_fileName = st;
                // Saving filecontent
                std::string::size_type bodyStart = str.find(lines[i + 1]) + lines[i + 1].length() + 3;
                std::string::size_type bodyEnd = this->_full_request.size() - this->_boundary.size() - 7;
                for(size_t i = 0; (bodyStart + i) < bodyEnd; i++)
                    this->_fileContent.push_back(this->_full_request[i + bodyStart]);
                // Printing the info we just get
                //std::cout << *this;
            }
        }
    }
}

std::ostream & operator<<(std::ostream &ost, const Request &src)
{
    ost << "-> METHOD: " << src.getMethod() << std::endl \
        << "-> ROUTE: " << src.getRoute() << std::endl \
        << "-> PROTOCOL: " << src.getProtocol() << std::endl \
        << "-> CONNECTION: " << src.getConnection() << std::endl \
        << "-> HOST: " << src.getHost() << std::endl \
        << "-> FILENAME: " << src.getFilename() << std::endl;
    ost << "-> FILECONTENT: ";
    for (size_t i = 0; i < src._fileContent.size(); i++)
        ost << src._fileContent[i];
    return ost;
}
