#pragma once
#include "Config.hpp"
#include <iostream>
#include <string>

class Client {
public:
    Client();
    Client(int socket_fd, int host, int port, ServerConfig config);
    ~Client();
    Client &operator=(Client const &obj);

    int socket_fd;
    int host;
    int port;

    ServerConfig config;
    
    std::string request;
    std::string response;
    size_t sent_size;
    time_t timeout;


    void process_request();

};