# include "server.h"
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <cstring>
# include <fstream>
# include <sstream>
# include <string>
# include <regex>

Server::Server(const ServerConfig& config) : config(config), server_fd(0) {}


void Server::start() {
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(config.port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(true) {
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        handleRequest(new_socket);
        close(new_socket);
    }
}

std::string Server::getMimeType(const std::string& filePath) {
    std::string extension = filePath.substr(filePath.find_last_of(".") + 1);
    printf("Detected extension: %s\n", extension.c_str());
    if (extension == "html" || extension == "mhtml") {
        return "text/html";
    } else if (extension == "css") {
        return "text/css";
    } else if (extension == "js") {
        return "text/javascript";
    } else if (extension == "jpg" || extension == "jpeg") {
        return "image/jpeg";
    } else if (extension == "png") {
        return "image/png";
    } else if (extension == "gif") {
        return "image/gif";
    } else if (extension == "svg") {
        return "image/svg+xml";
    } else {
        return "application/octet-stream";
    }

}

void Server::handleRequest(int client_socket) {
    char buffer[1024] = {0};
    recv(client_socket, buffer, 1024, 0);

    std::string request(buffer);
    std::string url;

    //printf("Request: %s\n", request.c_str());
    std::size_t methodEndPos = request.find(" ");
    if (methodEndPos != std::string::npos) {
        std::size_t urlEndPos = request.find(" ", methodEndPos + 1);
        if (urlEndPos != std::string::npos) {
            url = request.substr(methodEndPos + 1, urlEndPos - methodEndPos - 1);
        }
    }

    std::size_t lastSlashPos = url.find_last_of("/");
    std::size_t periodPos = url.find_last_of(".");
    if (periodPos == std::string::npos || periodPos < lastSlashPos) {
        url += config.htmlmode ? ".html" : ".mhtml";
    }

    std::string mimeType = getMimeType(url);
    printf("Mime type: %s\n For URL: %s\n", mimeType.c_str(), url.c_str());

    if (mimeType.substr(0, 5) == "text/" || mimeType == "application/javascript") {
        std::string htmlContent;
        printf("URL: %s\n", url.c_str());
        if (url == (config.htmlmode ? "/.html" : "/.mhtml")) {
            htmlContent = readMhtmlFile((config.htmlmode ? "/index.html" : "/index.mhtml"));
        } else {
            printf("URL: %s\n", url.c_str());
            htmlContent = readMhtmlFile(url);
        }

        std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(htmlContent.length()) + "\n\n" + htmlContent;
        send(client_socket, response.c_str(), response.length(), 0);
    }
    else {
        std::string fileContent = readFile(url);
        std::string response = "HTTP/1.1 200 OK\nContent-Type: " + mimeType + "\nContent-Length: " + std::to_string(fileContent.length()) + "\n\n" + fileContent;
        send(client_socket, response.c_str(), response.length(), 0);
    }
}

std::string Server::readFile(std::string pathTwo) {
    std::ifstream file(config.path + pathTwo, std::ios::binary);
    if (file) {
        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();
        return content;
    }else {
        printf("Could not open file %s\n", (config.path + pathTwo).c_str());
        return "<html><body><h1>404 Not Found</h1></body></html>";
    }
}


std::string Server::readMhtmlFile(std::string pathTwo) {
    if (pathTwo.substr(pathTwo.find_last_of(".") + 1) != "mhtml") {
       
    }
    printf("Reading file: %s\n", (config.path + pathTwo).c_str());
    std::ifstream file(config.path + pathTwo);
    std::stringstream buffer;
    if (file) {
        buffer << file.rdbuf();
        file.close();
    }else {
        printf("Could not open file %s\n", (config.path + pathTwo).c_str());
        return "<html><body><h1>404 Not Found</h1></body></html>";
    }

    std::string content = buffer.str();
    std::regex hlinkPattern("<hlink(\\s+.*?)?>\\s*(.*?)\\s*\\{\\{(.*?)\\}\\}\\s*</hlink>");
    std::string replacement = "<a href=\"$3\"$1 style=\"text-decoration: none;\">$2</a>";

    content = std::regex_replace(content, hlinkPattern, replacement);

    return content;
}

void Server::stop() {
    close(server_fd);
}