#include <string.h>
#include <unistd.h>
#include <sstream>
#include <fstream>

#include "Server.hpp"
#include "utils.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "web_server.hpp"

// ===== Constructor =====
Server::Server(Config conf)
{
	config = conf;
	this->_ip_address = config.host;
	this->_port = config.port;
	this->_socketAddress_len = sizeof(_socketAddress);
	this->_socketAddress.sin_family = AF_INET;
	this->_socketAddress.sin_port = htons(this->_port);
	this->_socketAddress.sin_addr.s_addr = inet_addr(this->_ip_address.c_str());
}

// ===== Destructor =====
Server::~Server()
{
	close(this->_socket);
}

// ===== Methods =====
int		Server::startServer(void)
{
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket < 0)
	{
		throw serverException("Cannot create socket");
		return (1);
	}

	if (bind(this->_socket, (sockaddr *)&this->_socketAddress, this->_socketAddress_len) < 0)
	{
		throw serverException("Cannot connect socket to address");
		return (1);
	}
	return (0);
}

int	Server::startListen(void)
{
	if (listen(this->_socket, 20) < 0)
	{
		throw serverException("Socket listen failed");
		return (1);
	}

	printMessage("===== Listening =====");
	printMessage("ADDRESS: " + std::string(inet_ntoa(this->_socketAddress.sin_addr)));
	std::cout << "PORT: " << ntohs(this->_socketAddress.sin_port) << std::endl;

	fd_set	current_sockets, ready_sockets;
	int		max_socket;

	// Initialize current set
	FD_ZERO(&current_sockets);
	FD_SET(this->_socket, &current_sockets);

	max_socket = this->_socket;

	while (1)
	{
		// Select is destructive
		ready_sockets = current_sockets;

		if (select(max_socket + 1, &ready_sockets, NULL, NULL, NULL) < 0)
		{
			throw serverException(strerror(errno));
			return (1);
		}
		for (int i = 0; i <= max_socket; i++)
		{
			if (!FD_ISSET(i, &ready_sockets))
				continue ; // write_set?
			if (i == this->_socket)
			{
				// New connection
				int new_socket = acceptConnection();
				FD_SET(new_socket, &current_sockets);
				if (new_socket > max_socket)
					max_socket = new_socket;
			}
			else
			{
				handleConnection(i);
				FD_CLR(i, &current_sockets);
			}
		}
	}
	return (0);
}

int	Server::acceptConnection(void)
{
	int	new_socket;

	new_socket = accept(this->_socket, (sockaddr *)&this->_socketAddress, &this->_socketAddress_len);
	if (new_socket < 0)
	{
		throw serverException("Server failed to accept connection");
		return (1);
	}
	return (new_socket);
}

int	Server::handleConnection(int client_socket)
{
	char	buffer[config.client_max_body_size];
	
	bzero(buffer, config.client_max_body_size);
	if (read(client_socket, buffer, config.client_max_body_size) < 0)
	{
		throw serverException("Cannot read request");
		return (1);
	}

	Request request(buffer);
	Response response(request);

	response.generateResponse();
	_serverResponse = response._fullResponse;

	sendResponse(client_socket);
	close(client_socket);
	printMessage("Closing connection");
	return (0);
}

// ===== Exception =====
Server::serverException::serverException(const char *error)
{
	this->_error = (char *)error;
	return ;
};
const char *Server::serverException::what() const throw()
{
	return (this->_error);
};



std::string Server::buildResponse()
{
	std::string filePath = "index.html"; // Ruta del archivo HTML

    std::ifstream file(filePath.c_str()); // Abrir el archivo en modo lectura
    if (!file)
    {
        throw serverException("Failed to open html file");
    }

    std::ostringstream fileContentStream;
    fileContentStream << file.rdbuf(); // Leer todo el contenido del archivo en un flujo de salida

    std::string fileContent = fileContentStream.str();

    std::ostringstream response;
    response << "HTTP/1.1 200 OK\n"
             << "Content-Type: text/html\n"
             << "Content-Length: " << fileContent.size() << "\n\n"
             << fileContent;

    return response.str();
}

void Server::sendResponse(int client_socket)
{
	unsigned long bytesSent;

	bytesSent = write(client_socket, _serverResponse.c_str(), _serverResponse.size());

	if (bytesSent != _serverResponse.size())
		printMessage("Error sending response to client");
}