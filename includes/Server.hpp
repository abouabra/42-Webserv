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

#define BUFFER_SIZE 1024
#define TIMEOUT 5

class Server {
public:
	Server(Config config);
	~Server();
	Server &operator=(Server const &obj);

	std::vector<int> server_fds;
	Config config;
	
    std::vector<Client> clients;

	fd_set master;
	fd_set reads;
	fd_set writes;
	struct timeval tv;
	int max_fd;

    void init();
    int create_server_socket();
    void set_socket_to_non_blocking(int socket_fd);
    void set_socket_to_be_reusable(int socket_fd);
    void bind_server_address(int socket_fd, ServerConfig server);
    void set_socket_to_listen(int socket_fd);
    void server_loop();
    void accept_connection(int socket_fd, int index);
    int read_from_client(int socket_fd, int index);
    int write_to_client(int socket_fd, int index);
    int check_for_timeout(Client client, int index);
    void close_connection(int socket_fd, int index);

};