#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "web_server.hpp"
// #include "Server.hpp"
# include <vector>
# include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>

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
		Config(std::string msg);
		// Destructor
		~Config();

		//void setBody(std::string &server);
		std::string getBody(void) const;
		const std::map<std::string, std::string> &getEnvironment() const;
    	void setEnv(char** env);
		void exportEnv(const std::string &key, const std::string &value);
		void fillFields(const std::string &src);
		void printEnv(void) const;
		void printConf(void) const;

	private:
		std::map<std::string, std::string> environment;
};

std::vector<std::string> extractServerBlocks(const std::string &filePath);

#endif