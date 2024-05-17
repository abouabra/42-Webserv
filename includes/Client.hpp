#pragma once
#include "Config.hpp"
#include <map>
#include <string>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdio>

#define CGI_TIMEOUT 30

class Client {
public:
    Client();
    Client(int socket_fd, int host, int port, ServerConfig config);
    ~Client();
    Client &operator=(Client const &obj);

    ServerConfig config;
    std::vector<std::string> env;

    // client variables
    int socket_fd;
    int host;
    int port;

    std::string request_file_name;
    int request_fd;
    bool write_to_file;

    bool parse_request_switch;
    bool should_send_headers;


    // Request variables
    std::string request;
    std::string method;
    std::string uri;
    std::string protocol;
    std::string request_host;
	std::string connection;
    std::string transfer_encoding;
    std::string content_length;
    std::string content_type;
    std::string cookie;
    std::string request_body;
    std::string request_query_string;
    time_t keep_alive_timeout;
    std::string body_size;

    // Response variables
    int response_status_code;
    std::string response_content_type;
    std::string response_body;
    std::string response;
    size_t sent_size;

    // maps for status codes and mime types
    std::map<int, std::string> status_codes;
    std::map<std::string, std::string> mime_types;

    void handle_request();
    void parse_request();
    void process_request();
    std::string check_error_page(int status_code);

    Client& set_status_code(int status_code);
    Client& set_body(std::string body);
    Client& set_content_type(std::string content_type);
    Client& set_body_size(std::string body_size);


    void build_response();
    bool check_request_validity();
    void decode_chunked_body();

    std::string generic_error_page(int status_code);
    void send_error_response(int status_code);
    int find_matching_location();
    bool process_location_redirection(int location_idx);
    bool validate_method_for_location(int location_idx);
    void process_GET_and_POST_and_HEAD(Location& location);
    void process_DELETE(Location& location);

    std::string construct_resource_path(Location& location);
    bool verify_resource_existence(std::string &resource_path);
    bool is_path_directory(std::string &resource_path);
    void process_directory(Location& location, std::string resource_path);
    void process_file(std::string resource_path, Location& location);
    bool has_uri_trailing_slash();
    void serve_directory_listing(std::string &resource_path);
    bool should_be_processed_by_cgi(std::string &resource_path);
    void serve_dynamic_content(std::string &resource_path, Location& location);
    void serve_static_content(std::string &resource_path);

    void process_GET_CGI(std::string &resource_path);
    void process_POST_CGI(std::string &resource_path, Location& location);
    void execute_CGI(const char *path, char *argv[], char *envp[]);

    void process_directory_for_DELETE(Location& location, std::string &resource_path);
    void process_file_for_DELETE(std::string resource_path, Location& location);
    void process_DELETE_CGI(std::string &resource_path);

    void delete_file(std::string &resource_path);

    int recursive_deletion(std::string path);

};