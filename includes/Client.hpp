#pragma once
#include "Config.hpp"
#include <iostream>
#include <map>
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
    std::string response_connection;
    std::string response_content_type;
    std::string response_body;
    std::string response;

    // maps for status codes and mime types
    std::map<int, std::string> status_codes;
    std::map<std::string, std::string> mime_types;

    void handle_request();
    void parse_request();
    void process_request();
    std::string check_error_page(int status_code);

    Client& set_status_code(int status_code);
    Client& set_connection(std::string connection);
    Client& set_body(std::string body);
    Client& set_content_type(std::string content_type);


    void build_response();
    bool check_request_validity();
    std::string generic_error_page(int status_code);
    void send_error_response(int status_code);
    int find_matching_location();
    bool process_location_redirection(int location_idx);
    bool validate_method_for_location(int location_idx);
    void process_GET(int location_idx);
    void process_POST(int location_idx);
    void process_DELETE(int location_idx);

};