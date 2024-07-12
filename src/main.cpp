#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>

#include "server.h"

bool fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

void createDefaultConfigFile(const std::string& path) {
    std::ofstream configFile(path);
    if (configFile.is_open()) {
        configFile << "port=8080\n";
        configFile << "path=tmp\n";
        configFile << "htmlmode=false\n";
        configFile.close();
        std::cout << "Default config file created: " << path << std::endl;
    } else {
        std::cerr << "Unable to create default config file: " << path << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::string configFilePath = "mconfig.txt";
    if (argc > 1) {
        configFilePath = argv[1];
    }

    if (!fileExists(configFilePath)) {
        createDefaultConfigFile(configFilePath);
    }

    ServerConfig config;

    config.port = 8080;
    config.path = "tmp";
    config.htmlmode = false;
    bool defaultsUsed = true;

    std::ifstream configFile(configFilePath);
    
    if (configFile.is_open()) {
        std::string line; 
        while(std::getline(configFile, line)) {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                if (key == "port") {
                    config.port = std::stoi(value);
                    defaultsUsed = false;
                } else if (key == "path") {
                    config.path = value;
                    defaultsUsed = false;
                } else if (key == "htmlmode"){
                    if (value == "true"){
                        config.htmlmode = true;
                    }
                    else{
                        config.htmlmode = false;
                    }
                } else {
                    std::cerr << "Unknown key: " << key << std::endl;
                }
            }
        }
        configFile.close();
    } else {
        std::cerr << "Unable to open config file. Using defaults." << std::endl;
    }

    struct stat info;
    if (!fileExists(config.path)) {
        std::cout << "Path " << config.path << " does not exist. Creating..." << std::endl;
        if (mkdir(config.path.c_str(), 0777) == -1) {
            std::cerr << "Error creating directory " << config.path << std::endl;
        } else {
            std::cout << "Directory created successfully." << std::endl;
            std::string filename = config.path + (config.htmlmode ? "/index.html" : "/index.mhtml");
            std::string defaultContent = "<h1>Welcome to MBHS!</h1>";
            std::ofstream file(filename);
            if (file.is_open()) {
                file << defaultContent;
                file.close();
                std::cout << "Default file created: " << filename << std::endl;
            } else {
                std::cerr << "Error creating file: " << filename << std::endl;
            }
        }
    } else if (!(info.st_mode & S_IFDIR)) { 
        std::cerr << "Path exists but is not a directory." << std::endl;
    }

    std::cout << "Using port: " << config.port << std::endl;
    std::cout << "Using path: " << config.path << std::endl;
    if (config.htmlmode){
        std::cout << "Using html mode" << std::endl;
    } else {
        std::cout << "Using mhtml mode" << std::endl;
    }

    //TODO add error handling if file broken or port taken


    Server server(config);
    try {
        server.start();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
