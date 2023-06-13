#ifndef SERVER_HPP
# define SERVER_HPP

#include <arpa/inet.h>

#include "Config.hpp"
#include "Response.hpp"

class Server
{
private:
	struct sockaddr_in	_socketAddress;
	unsigned int		_socketAddress_len;
	std::string			_ip_address;
	int					_socket;
	int					_port;
	std::string			_serverResponse;

public:
	// ===== Constructor =====
	Server(Config conf);

	// ===== Destructor =====
	~Server();

	// ===== Methods =====
	int	startServer(void);
	int	startListen(void);
	int	acceptConnection(void);
	int	handleConnection(int client_socket);
	std::string buildResponse(void);
	void sendResponse(int client_socket);

	// ===== Exception =====
	class serverException : public std::exception
    {
	private:
        char *_error;

    public:
		serverException(const char *error);
		virtual const char *what() const throw();
    };
};

#endif