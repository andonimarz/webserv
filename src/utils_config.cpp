#include "utils_config.hpp"

std::vector<std::string> setBody(std::string &path)
{
    std::vector<std::string>	conf_servers;
    std::ifstream file(path);
    if (!file)
        throw std::runtime_error("Error: File '" + path + "' not found.");
    std::stringstream buffer;
    buffer << file.rdbuf();
    // Expresión regular para buscar líneas que comienzan con "#"
    std::regex commentRegex("#.*");
    std::string conf_body;
    // Eliminar los comentarios reemplazando las coincidencias con una cadena vacía
    conf_body = std::regex_replace(buffer.str(), commentRegex, "");
    conf_servers = extractServerBlocks(conf_body);
    return conf_servers;
}

std::vector<std::string> extractServerBlocks(const std::string &configContent)
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
    return serverBlocks;
}
