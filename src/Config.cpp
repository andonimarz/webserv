#include "Config.hpp"

// Constructor
Config::Config()
{
	t_error_page page1;
	t_error_page page2;
	t_error_page page3;
	t_location location1;
	t_location location2;

	page1.n_error = 403;
	page1.path = "www/error/403.html";
	page2.n_error = 404;
	page2.path = "www/error/404.html";
	page3.n_error = 500;
	page3.path = "www/error/500.html";

	location1.method.push_back(GET);
	location1.path = "/";
	location1.root = "www";
	location1.index = "./index.html";

	location2.method.push_back(POST);
	location2.method.push_back(DELETE);
	location2.path = "/upload";
	location2.upload_enable = true;
	location2.upload_path = "www/uploads";

	this->port = 8080;
	this->host = "127.0.0.1";
	this->server_name.push_back("localhost");
	this->client_max_body_size = 500000;
	this->error_page.push_back(page1);
	this->error_page.push_back(page2);
	this->error_page.push_back(page3);
	this->location.push_back(location1);
	this->location.push_back(location2);
}

// Operator
Config	&Config::operator=(const Config &src)
{
	// Limpiar el contenido de this

	this->host = src.host;
	this->port = src.port;
	this->client_max_body_size = src.client_max_body_size;

	{
		// server_name vector
		std::vector<std::string>::iterator it = this->server_name.begin();
		while (it != this->server_name.end())
			it = this->server_name.erase(it);
		this->server_name.clear();
		for (size_t i = 0; i < src.server_name.size(); i++)
			this->server_name.push_back(src.server_name[i]);
	}
	{
		// location vector
		std::cout << "locationSize before: " << this->location.size() << std::endl;
		std::vector<t_location>::iterator it = this->location.begin();
		while (it != this->location.end())
			it = this->location.erase(it);
		this->location.clear();
		std::cout << "locationSize after: " << this->location.size() << std::endl;
		for (size_t i = 0; i < src.location.size(); i++)
			this->location.push_back(src.location[i]);
	}
	{
		// error_page vector
		std::vector<t_error_page>::iterator it = this->error_page.begin();
		while (it != this->error_page.end())
			it = this->error_page.erase(it);
		this->error_page.clear();
		for (size_t i = 0; i < src.error_page.size(); i++)
			this->error_page.push_back(src.error_page[i]);
	}
	return (*this);
}

Config::Config(const Config &src)
{
	*this = src;
}

Config::Config(std::string msg)
{
	std::cout << msg << std::endl;
	this->port = 0;
	//Sobreescribir host en fillFields
	this->host = "";
	this->client_max_body_size = 0;
}

// Destructor
Config::~Config() {}

std::string Config::getBody(void) const
{
	return this->conf_body;
}

void Config::setEnv(char **env)
{
	// Recorrer el arreglo de punteros a char hasta encontrar el elemento nulo (final)
	for (int i = 0; env[i] != nullptr; i++)
	{
		// Separar la cadena en la forma "clave=valor"
		std::string envStr(env[i]);
		size_t equalsPos = envStr.find('=');
		if (equalsPos != std::string::npos)
		{
			std::string key = envStr.substr(0, equalsPos);
			std::string value = envStr.substr(equalsPos + 1);
			environment[key] = value;
		}
	}
}

const std::map<std::string, std::string> &Config::getEnvironment() const
{
	return environment;
}

void Config::printEnv(void) const
{
	for (std::map<std::string, std::string>::const_iterator it = this->environment.begin(); it != environment.end(); ++it)
		std::cout << it->first << "=" << it->second << std::endl;
}

void Config::exportEnv(const std::string &key, const std::string &value)
{
	environment[key] = value;
}

void Config::fillFields(const std::string &src)
{
	std::istringstream iss(src);
	std::vector<std::string> lines;
	std::string line;
	bool inLocation = 0;
	std::size_t locationIndex = 0;

	// Leer las líneas del texto y almacenarlas en un deque
	while (std::getline(iss, line))
		lines.push_back(line);

	// Recorrer las líneas y buscar coincidencias en la primera palabra
	for (std::size_t i = 0; i < lines.size(); ++i)
	{
		if (lines[i].empty() || (lines[i].length() <= 2 && lines[i].find("}") == std::string::npos))
			continue;
		std::istringstream lineStream(lines[i]);
		std::vector<std::string> words;
		std::string word;
		// Descomponemos la linea en palabras
		while (lineStream >> word)
		{
			// Tanto ; como # se interpretan como comentarios
			if (word.find(";") != std::string::npos || word.find("#") != std::string::npos)
			{
				word = word.substr(0, word.find(";"));
				word = word.substr(0, word.find("#"));
				words.push_back(word);
				break;
			}
			if (!word.empty())
				words.push_back(word);
		}
		// Igual tenemos que meter uno como este para cada parametro
		if (words.size() > 3)
			throw std::runtime_error("Error: too many fields");
		// Agregar un comentario para cada palabra clave encontrada
		if (words[0] == "listen")
		{
			std::istringstream iss;
			// minitest
			if (words.size() != 2)
				throw std::runtime_error("Error: invalid host");
			size_t pos = words[1].find(":");
			if( pos == std::string::npos)
				iss.str(words[1]);
			else
			{
				std::string ipAddress = words[1].substr(0, pos);
				if (isValidIPAddress(ipAddress))
					this->host = ipAddress;
				else
					throw std::runtime_error("Error: bad ip address");
				iss.str(words[1].substr(pos + 1));
			}
			int result;
			if (!(iss >> result))
				throw std::runtime_error("Error: bad port");;
			this->port = result;
		}
		else if (words[0] == "server_name")
		{
			for (i = 1; i < words.size(); i++)
			{
				if (words[i] == "localhost")
					words[i] = "127.0.0.1";
				std::vector<std::string>::iterator it = std::find(server_name.begin(), server_name.end(), words[i]);
				if (it == server_name.end() || it->size() != words[i].size())
					this->server_name.push_back(words[i]);
			}
		}
		else if (words[0] == "location" || inLocation)
		{
			if (!inLocation)
			{
				// Comprobamos que se cierren los corchetes correctamente
				for (std::size_t j = i + 1; lines[j].find("}") == std::string::npos; j++)
					if (j == lines.size() - 1 || lines[j].find("location") != std::string::npos)
						throw std::runtime_error("Error: location {} not closed");
				t_location tmp;
				tmp.path = words[1];
				this->location.push_back(tmp);
				locationIndex = location.size() - 1;
				inLocation = 1;
			}
			else
			{
				if (words[0] == "method")
				{
					if (words[1] == "GET")
						this->location[locationIndex].method.push_back(GET);
					else if (words[1] == "POST")
						this->location[locationIndex].method.push_back(POST);
					else if (words[1] == "DELETE")
						this->location[locationIndex].method.push_back(DELETE);
					else
						throw std::runtime_error("Error: bad location method");
				}
				else if (words[0] == "root")
					this->location[locationIndex].root = words[1];
				else if (words[0] == "index")
					this->location[locationIndex].index = words[1];
				else if (words[0] == "upload_path")
					this->location[locationIndex].upload_path = words[1];
				else if (words[0] == "cgi_extension")
					this->location[locationIndex].cgi_extension = words[1];
				else if (words[0] == "cgi_path")
					this->location[locationIndex].cgi_path = words[1];
				else if (words[0] == "autoindex")
				{
					if (words[1] == "on" || words[1] == "ON")
						this->location[locationIndex].autoindex = 1;
					if (words[1] == "off" || words[1] == "OFF")
						this->location[locationIndex].autoindex = 0;
				}
				else if (words[0] == "upload_enable")
				{
					if (words[1] == "true" || words[1] == "TRUE")
						this->location[locationIndex].upload_enable = 1;
					if (words[1] == "false" || words[1] == "FALSE")
						this->location[locationIndex].upload_enable = 0;
				}
				if (lines[i].find("}") != std::string::npos)
					inLocation = 0;
			}
		}
		else if (words[0] == "error_page")
		{
			// minitest
			if (words.size() != 3)
				throw std::runtime_error("Error: bad error page");
			for (size_t i = 0; i < words[1].size(); i++)
				if (!std::isdigit(words[1][i]))
					throw std::runtime_error("Error: bad error number");
			if (access(words[2].c_str(), F_OK) == -1)
				throw std::runtime_error("Error: invalid error path");
			// getting params
			t_error_page tmp;
			std::istringstream iss(words[1]);
			int result;
			iss >> result;
			tmp.n_error = result;
			tmp.path = words[2];
			this->error_page.push_back(tmp);
		}
		else if (words[0] == "client_max_body_size")
		{
			// minitest
			if (words.size() != 2)
				throw std::runtime_error("Error: bad max body size param number");
			if (words[1].empty())
				throw std::runtime_error("Error: empty max body size number");
			for (size_t i = 0; i < words[1].size(); i++)
				if (!std::isdigit(words[1][i]) && !(i == words[1].size() - 1 && words[1][i] == 'K'))
					throw std::runtime_error("Error: bad max body size params");
			// getting the number
			int multiplier = 1;
			if (words[1].back() == 'K')
			{
				multiplier = 1000;
				words[1].pop_back();
			}
			int num = std::stoi(words[1]);
			this->client_max_body_size = multiplier * num;
		}
	}
}

void Config::printConf(void) const
{
	std::cout << "===== PRINTING CONF PARAMETERS =====" << std::endl;
	std::cout << "host: " << this->host << std::endl
			  << "port: " << this->port << std::endl
			  << "client_max_body_size: " << this->client_max_body_size << std::endl;
	std::cout << "server_names: \n";
	for (size_t i = 0; i < this->server_name.size(); i++)
		std::cout << "[" << i << "]"<< server_name[i] << std::endl;
	for (size_t i = 0; i < this->location.size(); i++)
	{
		std::cout << "location[" << i << "]:\n";
		for(size_t j = 0; j < this->location[i].method.size(); j++)
			std::cout << "-> method[" << j << "]: "<< this->location[i].method[j] << std::endl;
		std::cout << "-> path: " << this->location[i].path << std::endl \
		<< "-> root: " << this->location[i].root << std::endl \
		<< "-> index: " << this->location[i].index << std::endl \
		<< "-> upload_path: " << this->location[i].upload_path << std::endl \
		<< "-> upload_enable: " << this->location[i].upload_enable << std::endl \
		<< "-> cgi_extension: " << this->location[i].cgi_extension << std::endl \
		<< "-> cgi_path: " << this->location[i].cgi_path << std::endl \
		<< "-> autoindex: " << this->location[i].autoindex << std::endl;
	}
	for (size_t i = 0; i < this->error_page.size(); i++)
	{
		std::cout << "error_page[" << i << "]" << std::endl;
		std::cout << "-> path: " << this->error_page[i].path << std::endl;
		std::cout << "-> n_error: " << this->error_page[i].n_error << std::endl;
	}
	std::cout << "=========================================" << std::endl;
}
