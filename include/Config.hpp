#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <vector>
# include <iostream>
# include <fstream>
# include <sstream>
# include <regex>

# include "web_server.hpp"

// En esta clase se parsea y guardan todos los datos extraidos de un archivo de configuración
class Config
{

	// Estructuras de datos de un servidor
	public:
		typedef struct s_error_page
		{
			std::string	path;
			int			n_error;
		}				t_error_page;

		typedef struct s_location
		{
			std::vector<int>	method;
			std::string			path;
			std::string			root;
			std::string			index;
			std::string			upload_path;
			std::string			cgi_extension;
			std::string			cgi_path;
			bool				autoindex;
			bool				upload_enable;
		}				t_location;

	public:
		std::vector<t_error_page>	error_page;
		std::vector<t_location>		location;
		std::vector<std::string>	server_name;
		std::string					host;
		int							port;
		int							client_max_body_size;
		std::string					conf_body;

	public:
		// Constructor
		Config();
		// Destructor
		~Config();

		//void setBody(std::string &server);
		std::string getBody(void) const;
};

std::vector<std::string> extractServerBlocks(const std::string &filePath);

#endif