#include "Route.hpp"

#include <string.h>
// ===== Constructors =====
Route::Route(Config conf)
{
	this->_server_list.push_back(Server(conf));
	this->_server_list[0].startServer();
	return ;
}
Route::Route(std::vector<Config> config_list)
{
	for (int i = 0; i < (int)config_list.size(); i++)
		this->_server_list.push_back(Server(config_list[i]));
	for (int i = 0; i < (int)this->_server_list.size(); i++)
		this->_server_list[i].startServer();
	return ;
}
// ===== Destructor =====
Route::~Route(){ return ; }

// Initialize server sockets
int	Route::startListen(void)
{
	int		j;

	FD_ZERO(&this->_currentSockets);
	for (j = 0; j < (int)this->_server_list.size(); j++)
		FD_SET(this->_server_list[j].getSocket(), &this->_currentSockets);

	this->_maxSocket = this->_server_list[j - 1].getSocket();
	this->selectRequest();
	return (0);
}

// Wait socket for read or write
int	Route::selectRequest(void)
{
	fd_set	ready_sockets;

	while (1)
	{
		ready_sockets = this->_currentSockets;
		if (select(this->_maxSocket + 1, &ready_sockets, NULL, NULL, NULL) < 0)
		{
			throw Server::serverException(strerror(errno));
			return (1);
		}
		for (int i = 0; i <= this->_maxSocket; i++)
		{
			if (!FD_ISSET(i, &ready_sockets))
				continue ;
			this->redirectRequest(i);
		}
	}
	return (0);
}

// Select the corresponding server, then accept or handle the socket
int	Route::redirectRequest(int socket_selected)
{
	for (int j = 0; j < (int)this->_server_list.size(); j++)
	{
		if (socket_selected == this->_server_list[j].getSocket())
		{
			int new_socket = this->_server_list[j].acceptConnection();
			FD_SET(new_socket, &this->_currentSockets);
			if (new_socket > this->_maxSocket)
				this->_maxSocket = new_socket;
			this->_server_list[j].pushClientSocket(new_socket);
			break ;
		}
		else
		{
			int k;
			for (k = 0; k < this->_server_list[j].getSizeClientSockets(); k++)
			{
				if (socket_selected != this->_server_list[j].getClientSocket(k))
					continue ;
				try
				{
					this->_server_list[j].handleConnection(socket_selected);
				}
				catch (std::exception &e)
				{
					std::cerr << e.what() << std::endl;
				}
				this->_server_list[j].deleteClientSocket(socket_selected);
				FD_CLR(socket_selected, &this->_currentSockets);
			}
			if (k < this->_server_list[j].getSizeClientSockets())
				break ;
		}
	}
	return (0);
}