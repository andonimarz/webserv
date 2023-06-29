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

	initLocation(location1);
	location1.method.push_back(GET);
	location1.path = "/";
	location1.root = "www";
	location1.index = "index.html";

	initLocation(location2);
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
		std::vector<t_location>::iterator it = this->location.begin();
		while (it != this->location.end())
			it = this->location.erase(it);
		this->location.clear();
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

// ===== Getters =====
Config::t_error_page	Config::getErrorPage(int index)
{
	if (index < (int)this->error_page.size())
		return (this->error_page[index]);
	t_error_page	empty;
	return (empty);
}
std::string	Config::getHost(void)
{
	return (this->host);
}
Config::t_location	Config::getLocation(int index)
{
	if (index < (int)this->location.size())
		return (this->location[index]);
	t_location	empty;
	return (empty);
}
int	Config::getPort(void)
{
	return (this->port);
}
int	Config::getClientMaxBodySize(void)
{
	return (this->client_max_body_size);
}
int	Config::getSizeLocation(void)
{
	return ((int)this->location.size());
}
size_t	Config::getSizeServerName(void)
{
	return (this->server_name.size());
}
std::vector<Config::t_error_page>	Config::getErrorPages(void)
{
	return (this->error_page);
}
std::vector<Config::t_location>		Config::getLocations(void)
{
	return (this->location);
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
	std::vector<std::string> firstWords;
	std::vector<std::string> locationWords;
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
		std::vector<std::string> words = fillWords(lines[i]);
		// Comprobar que no haya campos duplicados
		std::vector<std::string>::const_iterator it;
		if (words[0] != "location" && words[0] != "error_page")
		{
			if (inLocation == 0)
			{
				for (it = firstWords.begin(); it != firstWords.end(); ++it)
					if (*it == words[0])
						throw std::runtime_error("Error: field '" + words[0] + "' duplicated.");
				firstWords.push_back(words[0]);
			}
			else
			{
				if (words[0] != "method")
					for (it = locationWords.begin(); it != locationWords.end(); ++it)
						if (*it == words[0])
							throw std::runtime_error("Error: field '" + words[0] + "' duplicated in location.");
				locationWords.push_back(words[0]);
			}
		}
		if (words[0] == "server")
			continue;
		// Agregar un comentario para cada palabra clave encontrada
		if (words[0] == "listen")
			saveListen(words);
		else if (words[0] == "server_name")
			saveServerName(words);
		else if (words[0] == "error_page")
			saveErrorPage(words);
		else if (words[0] == "client_max_body_size")
			saveMaxSize(words);
		else if (words[0] == "location" || inLocation)
		{
			if (words.size() > 3)
				throw std::runtime_error("Error: too many fields");
			if (!inLocation)
			{
				// Comprobamos que se cierren los corchetes correctamente
				for (std::size_t j = i + 1; lines[j].find("}") == std::string::npos; j++)
					if (j == lines.size() - 1 || lines[j].find("location") != std::string::npos)
						throw std::runtime_error("Error: location {} not closed");
				// Creamos un location y lo pusheamos al vector
				t_location tmp;
				initLocation(tmp);
				tmp.path = words[1];
				this->location.push_back(tmp);
				locationIndex = location.size() - 1;
				inLocation = 1;
			}
			else
			{
				saveLocation(words, locationIndex);
				if (lines[i].find("}") != std::string::npos)
				{
					if (location[locationIndex].autoindex == true)
						location[locationIndex].index = "index.html";
					inLocation = 0;
					locationWords.clear();
				}
			}
		}
		else
			throw std::runtime_error("Error: unknown field parameter '" + words[0] + "'");
	}
}

std::vector<std::string> Config::fillWords(std::string line)
{
	std::istringstream lineStream(line);
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
	return words;
}

void Config::saveMaxSize(std::vector<std::string> &words)
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

void Config::saveErrorPage(std::vector<std::string> &words)
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

void Config::initLocation(Config::t_location &src)
{
	src.path = "";
    src.root = "";
    src.index = "";
    src.upload_path = "";
    src.cgi_extension = "";
    src.cgi_path = "";
    src.autoindex = true;
    src.upload_enable = false;
}

void Config::saveLocation(std::vector<std::string> &words, std::size_t &locationIndex)
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
	else if (words[0] == "autoindex")
	{
		if (words[1] == "on" || words[1] == "ON")
			this->location[locationIndex].autoindex = true;
		if (words[1] == "off" || words[1] == "OFF")
		{
			this->location[locationIndex].index = "";
			this->location[locationIndex].autoindex = false;
		}
	}
	else if (words[0] == "root")
		this->location[locationIndex].root = words[1];
	else if (words[0] == "index")
	{
		this->location[locationIndex].autoindex = false;
		this->location[locationIndex].index = words[1];
	}
	else if (words[0] == "upload_path")
		this->location[locationIndex].upload_path = words[1];
	else if (words[0] == "cgi_extension")
		this->location[locationIndex].cgi_extension = words[1];
	else if (words[0] == "cgi_path")
		this->location[locationIndex].cgi_path = words[1];
	else if (words[0] == "upload_enable")
	{
		if (words[1] == "on" || words[1] == "ON")
			this->location[locationIndex].upload_enable = true;
		if (words[1] == "off" || words[1] == "OFF")
			this->location[locationIndex].upload_enable = false;
	}
}

void Config::saveServerName(std::vector<std::string> &words)
{
	/* // Max 3 server names. POR REVISAR!
	if (words.size() > 4)
			throw std::runtime_error("Error: too many fields");
	for (size_t i = 1; i < words.size(); i++)
	{
		if (words[i] == "localhost")
			words[i] = "127.0.0.1";
		std::vector<std::string>::iterator it = std::find(server_name.begin(), server_name.end(), words[i]);
		if (it == server_name.end() || it->size() != words[i].size())
			this->server_name.push_back(words[i]);
	} */
	// Max 1 server name and must be localhost.
	if (words.size() != 2)
		throw std::runtime_error("Error: obligatory one server name per server.");
	if (words[1] != "localhost" && words[1] != "127.0.0.1")
		throw std::runtime_error("Error: only localhost or 127.0.0.1 allowed as server name.");
	this->server_name.push_back("127.0.0.1");
	this->host = "127.0.0.1";
}

void Config::saveListen(const std::vector<std::string> &words)
{
	std::istringstream iss;
	// minitest. listen solo va a tener dos palabras. listen y el puerto
	if (words.size() != 2)
		throw std::runtime_error("Error: invalid host");
	size_t pos = words[1].find(":");
	// si no hay dos puntos, guardamos tal cual el resultado
	if( pos == std::string::npos)
		iss.str(words[1]);
	else
	{
		// si hay dos puntos separamos el host del puerto y los guardamos
		std::string ipAddress = words[1].substr(0, pos);
		/* if (isValidIPAddress(ipAddress))
			this->host = ipAddress;
		 */
		if (ipAddress == "127.0.0.1")
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
