#include "Config.hpp"

// Constructor
Config::Config()
{
	{
		t_error_page	page1;
		t_error_page	page2;
		t_error_page	page3;
		t_location		location1;
		t_location		location2;
		
		page1.n_error = 404;
		page1.path = "www/error/404.html";
		page2.n_error = 405;
		page2.path = "www/error/405.html";
		page3.n_error = 406;
		page3.path = "www/error/406.html";

		location1.method.push_back(GET);
		location1.path = "/";
		location1.root = "www";
		location1.index = "./index.html";
		
		location2.method.push_back(POST);
		location2.path = "/upload";
		location2.upload_enable = true;
		location2.upload_path = "www/uploads";

		this->port = 8080;
		this->host = "0.0.0.0";
		this->server_name.push_back("localhost");
		this->client_max_body_size = 500000;
		this->error_page.push_back(page1);
		this->error_page.push_back(page2);
		this->error_page.push_back(page3);
		this->location.push_back(location1);
		this->location.push_back(location2);
	}
}

// Destructor
Config::~Config() {}

/* void Config::setBody(std::string &server)
{
	// Rellenar campos + tests desde el vector de strings
    
} */

std::string Config::getBody(void) const
{
	return this->conf_body;
}


void Config::setEnv(char** env)
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
		if (lines[i].empty() || lines[i].length() <= 2)
			continue;
        //std::cout << lines[i].length() << "========" << lines[i] << std::endl;
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
		//std::cout << words[0] << " -> " << words[1] << std::endl;
		if (words.size() > 3)
			throw std::runtime_error("Error: Missing config fields");
        // Agregar un comentario para cada palabra clave encontrada
        if (words[0] == "listen")
        {
			std::istringstream iss(words[1]);
    		int result;
    		iss >> result;
			this->port = result;
			std::cout << "Port = " << this->port << std::endl;
        }
        else if (words[0] == "server_name")
        {
            for (i = 1; i < words.size() ; i++)
			{
				this->server_name.push_back(words[i]);
				std::cout << "Server_name = " << words[i] << std::endl;
			}
        }
        else if (words[0] == "location" || inLocation)
        {
			if (!inLocation)
			{
				// Comprobamos que se cierren los corchetes correctamente
				for (std::size_t j = i + 1 ; lines[j].find("}") == std::string::npos; j++)
					if (j == lines.size() - 1 || lines[j].find("location") != std::string::npos)
						throw std::runtime_error("Error: location {} not closed");
				t_location tmp;
				tmp.path = words[1];
				this->locations.push_back(tmp);
				locationIndex = locations.size() - 1;
				inLocation = 1;
			}
			else
			{
				if (words[0] == "method")
				{
					if(words[1] == "GET")
						this->locations[locationIndex].method.push_back(GET);
					else if(words[1] == "POST")
						this->locations[locationIndex].method.push_back(POST);
					else if(words[1] == "DELETE")
						this->locations[locationIndex].method.push_back(DELETE);
					else
						throw std::runtime_error("Error: bad location method");
				}
				else if (words[0] == "root")
					this->locations[locationIndex].root = words[1];
				else if (words[0] == "index")
					this->locations[locationIndex].index = words[1];
				else if (words[0] == "upload_path")
					this->locations[locationIndex].upload_path = words[1];
				else if (words[0] == "cgi_extension")
					this->locations[locationIndex].cgi_extension = words[1];
				else if (words[0] == "cgi_path")
					this->locations[locationIndex].cgi_path = words[1];
				else if (words[0] == "autoindex")
				{
					if (words[1] == "on" || words[1] == "ON")
						this->locations[locationIndex].autoindex = 1;
					if (words[1] == "off" || words[1] == "OFF")
						this->locations[locationIndex].autoindex = 0;
				}
				else if (words[0] == "upload_enable")
				{
					if (words[1] == "true" || words[1] == "TRUE")
						this->locations[locationIndex].upload_enable = 1;
					if (words[1] == "false" || words[1] == "FALSE")
						this->locations[locationIndex].upload_enable = 0;
				}

				if (lines[i].find("}") != std::string::npos){
					inLocation = 0;
				}			
			}
        }
        else if (words[0] == "error_page")
        {
            if (words.size() != 3)
				throw std::runtime_error("Error: bad error page");
			t_error_page tmp;
			std::istringstream iss(words[1]);
    		int result;
    		iss >> result;
			tmp.n_error = result;
			tmp.path = words[2];
			this->error_page.push_back(tmp);
			std::cout << "error_page = " << std::endl \
				<< "-> n_error = " << tmp.n_error << std::endl \
				<< "-> path = " << tmp.path << std::endl;
        }
    }
	for (std::size_t index = 0; index < locations.size() ; index++)
	{
		std::cout << "location = " << std::endl \
			<< "-> path = " << locations[index].path << std::endl;
		for(size_t i = 0; i < locations[index].method.size(); i++)
			std::cout << "-> method[" << i << "] = " << locations[index].method[i] << std::endl;
		std::cout << "-> root = " << locations[index].root << std::endl\
			<< "-> index = " << locations[index].index << std::endl\
			<< "-> upload_path = " << locations[index].upload_path << std::endl\
			<< "-> cgi_extension = " << locations[index].cgi_extension << std::endl\
			<< "-> cgi_path = " << locations[index].cgi_path << std::endl\
			<< "-> autoindex = " << locations[index].autoindex << std::endl\
			<< "-> upload_enable = " << locations[index].upload_enable << std::endl;
	}
}
