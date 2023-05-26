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
		page1.path = "./404.html";
		page2.n_error = 405;
		page2.path = "./405.html";
		page3.n_error = 406;
		page3.path = "./406.html";

		location1.method.push_back(GET);
		location1.path = "/";
		location1.root = "/www";
		location1.index = "./homepage.html";
		
		location2.method.push_back(POST);
		location2.path = "/upload";
		location2.upload_enable = true;
		location2.upload_path = "/tmp/uploads/";

		this->port = 8080;
		this->host = "0.0.0.0";
		this->server_name.push_back("localhost");
		this->client_max_body_size = 500000;
		this->error_page.push_back(page1);
		this->error_page.push_back(page2);
		this->error_page.push_back(page3);
		this->location.push_back(location1);
		this->location.push_back(location2);
		this->conf_body = "";
		this->conf_servers = std::vector<std::string>();
	}
}

// Destructor
Config::~Config() {}

void Config::setBody(std::string &path)
{
	std::ifstream file(path);
	if (!file)
		throw std::runtime_error("Error: File '" + path + "' not found.");
    std::stringstream buffer;
    buffer << file.rdbuf();
	// Expresión regular para buscar líneas que comienzan con "#"
    std::regex commentRegex("#.*");

    // Eliminar los comentarios reemplazando las coincidencias con una cadena vacía
	this->conf_body = std::regex_replace(buffer.str(), commentRegex, "");

	this->conf_servers = extractServerBlocks(this->conf_body);

}

std::string Config::getBody(void) const
{
	return this->conf_body;
}

// WORK IN PROGRESS
std::vector<std::string> extractServerBlocks(const std::string& configContent)
{
    std::vector<std::string> serverBlocks;

    std::size_t pos = 0;
    std::size_t serverStart = 0;
    int braceCount = 0;
    int locationBraceCount = 0;
    bool insideLocation = false;

    while (pos != std::string::npos) {
        std::size_t start = configContent.find("server {", pos);
        std::size_t end = configContent.find("}", pos);

        if (start != std::string::npos && start < end) {
            if (!insideLocation && braceCount == 0) {
                serverStart = start;
            }
            braceCount++;
            pos = start + 1;
        } else if (end != std::string::npos) {
            braceCount--;
            if (!insideLocation && braceCount == 0) {
                std::size_t serverEnd = end + 1;
                serverBlocks.push_back(configContent.substr(serverStart, serverEnd - serverStart));
            }
            pos = end + 1;
        } else {
            break;
        }

        // Check if we are inside a location block
        std::size_t locationStart = configContent.find("location", pos);
        std::size_t locationEnd = configContent.find("{", pos);
        std::size_t locationClose = configContent.find("}", pos);

        if (locationStart != std::string::npos && (locationStart < locationEnd || locationEnd == std::string::npos)) {
            insideLocation = true;
            locationBraceCount++;
            pos = locationEnd + 1;
        } else if (locationClose != std::string::npos) {
            locationBraceCount--;
            if (locationBraceCount == 0) {
                insideLocation = false;
            }
            pos = locationClose + 1;
        }
    }
	for (const auto& element : serverBlocks)
    	std::cout << "Element: " << element << std::endl;
    return serverBlocks;
}