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
    
    size_t sent_size;
    time_t timeout;


    // Request Parameters
    std::string request;
    std::string method;
    std::string uri;
    std::string protocol;
    std::string request_host;
	std::string connection;
    std::string content_length;
    std::string content_type;
    std::string cookie;
    std::string request_body;


    // Response Parameters
    int response_status_code;
    std::string response_reason_phrase;
    std::string response_content_type;
    std::string response_connection;
    std::string response_body;
    std::string response;


    void handle_request();
    void parse_request();
    void process_request();

    Client& set_status_code(int status_code);
    Client& set_reason_phrase(std::string reason_phrase);
    Client& set_content_type(std::string content_type);
    Client& set_connection(std::string connection);
    Client& set_body(std::string body);

    void build_response();
};