#pragma once
#include <vector>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <map>
#include <iostream>
#include "Config.hpp"
#include "Client.hpp"
#include "WebServ.hpp"


class Server {
public:
	Server(Config config, char **ev);
	~Server();
	Server &operator=(Server const &obj);

	std::vector<int> server_fds;
    std::map<int, ServerConfig> server_config_indexs;
	Config config;

    std::vector<Client> clients;
	fd_set master;
	fd_set reads;
	fd_set writes;
	struct timeval tv;
	int max_fd;

    std::vector<std::string> env;
    
    void init();
    void convert_env_to_vector(char **env);
    int create_server_socket();
    void set_socket_to_non_blocking(int socket_fd);
    void set_socket_to_be_reusable(int socket_fd);
    void bind_server_address(int socket_fd, ServerConfig server);
    void set_socket_to_listen(int socket_fd);
    void server_loop();
    void accept_connection(int socket_fd);
    void read_from_client(int socket_fd, int index);
    void write_to_client(int socket_fd, int index);
    void check_for_timeout(Client& client, int index);
    void close_connection(int socket_fd, int index);
    int should_make_socket_for_server(ServerConfig server, int i);
};