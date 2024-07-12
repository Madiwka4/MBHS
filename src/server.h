#ifndef SERVER_H 
#define SERVER_H

#include <iostream>
#include <string> 

struct ServerConfig {
    int port;
    std::string path;
    bool htmlmode;
};

class Server {
    public:
        Server(const ServerConfig& config);
        void start();
        void stop();

    private:
        int server_fd;
        ServerConfig config;
        void handleRequest(int client_socket);
        std::string readMhtmlFile(std::string pathTwo);
        std::string readFile(std::string pathTwo);
        std::string getMimeType(const std::string& filePath);
};

#endif