#include "../includes/Server.hpp"
#include <cstddef>
#include <cstring>
#include <ctime>
#include <netinet/in.h>
#include <sys/fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(Config config, char **ev) : config(config) {
    this->max_fd = 0;
	tv.tv_sec = SELECT_TIMEOUT;
	tv.tv_usec = 0;
	convert_env_to_vector(ev);
}

Server::~Server() {
}

Server &Server::operator=(Server const &obj) {
    if (this != &obj) {
        this->config = obj.config;
        this->server_fds = obj.server_fds;
		this->clients = obj.clients;
		this->master = obj.master;
		this->reads = obj.reads;
		this->writes = obj.writes;
		this->max_fd = obj.max_fd;
		this->tv = obj.tv;
    }
    return *this;
}

void Server::convert_env_to_vector(char **env) {
	// here we convert the environment variables to a vector

	// we loop through the environment variables
	for (size_t i = 0; env[i] != NULL; i++) {
		// we add the environment variable to the env vector
		this->env.push_back(env[i]);
	}
}


void Server::init() {

	/*
		here is the master function that will be called to initialize the server
		it will create a socket for each server in the config
		it will set the sockets to non-blocking
		then set the sockets to be reusable
		then bind the sockets to the server address
		then listen on the sockets

	*/

	// we initialize the fd_set variables
	FD_ZERO(&this->master);
	FD_ZERO(&this->reads);
	FD_ZERO(&this->writes);

    // here we loop through the servers in the config
    for (size_t i=0; i < this->config.servers.size(); i++) {

		// here we create a socket for each server
		int socket_fd = create_server_socket();
		
		// we add the socket to the list of server sockets
        this->server_fds.push_back(socket_fd);

		// set the socket to non-blocking
		set_socket_to_non_blocking(socket_fd);

		// set the socket to be reusable
		set_socket_to_be_reusable(socket_fd);

		// bind the socket to the server address
		bind_server_address(socket_fd, this->config.servers[i]);

		// set the socket to listen for incoming connections
		set_socket_to_listen(socket_fd);

		// we add the socket to the master fd_set
		FD_SET(socket_fd, &this->master);

		// we check if the socket_fd is greater than the max_fd
		// if so, we set the max_fd to the socket_fd
		if (socket_fd > this->max_fd) {
			this->max_fd = socket_fd;
		}

		// we log that the server has started
		log("Server started on: http://" + int_to_ip(this->config.servers[i].host) + ":" + itoa(this->config.servers[i].port[0]), WHITE);
	}

	server_loop();

}    

int Server::create_server_socket()
{
	// here we create a socket using the following parameters:
	// AF_INET: IPv4
	// SOCK_STREAM: TCP
	// 0: protocol (default)
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	// if the socket creation failed, we throw an exception
	if (socket_fd < 0) {
		throw std::runtime_error("socket creation failed");
	}

	// we return the socket file descriptor
	return socket_fd;
}

void Server::set_socket_to_non_blocking(int socket_fd)
{
	// here we set the socket_fd to non-blocking mode
	// non blocking sockets are sockets that do not hold up the program when they are waiting for data

	// we set the socket to non-blocking by adding the O_NONBLOCK and O_CLOEXEC flags
	// we use the OR operator to add the flag
	// O_NONBLOCK: non-blocking mode
	// O_CLOEXEC: close the file descriptor when an exec function is called to prevent file descriptor leaks
	
	int result = fcntl(socket_fd, F_SETFL, O_NONBLOCK | O_CLOEXEC);
	// if the result is less than 0, we throw an exception
	if (result < 0)
		throw std::runtime_error("failed to set socket to non-blocking");
}

void Server::set_socket_to_be_reusable(int socket_fd)
{
	// here we set the socket to be reusable
	// reausable sockets can be used again after they have been closed or the program has been terminated

	// we create an integer variable to store the value of the SO_REUSEADDR option
	// we need it because the setsockopt function requires a pointer to the value
	int opt = 1;

	// we set the socket option using setsockopt with the following parameters:
	// socket_fd: the socket file descriptor
	// SOL_SOCKET: socket level
	// SO_REUSEADDR: the option that allows the socket to be reused
	// &opt: the value of the option (we set it to 1 because we want to enable the option)
	// sizeof(opt): the size of the option (size of an integer)
	int result = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	// if the result is less than 0, we throw an exception
	if (result < 0)
		throw std::runtime_error("failed to set socket to be reusable");
}

void Server::bind_server_address(int socket_fd, ServerConfig server)
{
	// here we bind the socket to the server address
	// we need to create a sockaddr_in struct to store the server address
	struct sockaddr_in server_address;

	// we set the server address to 0 using memset to clear any garbage data
	std::memset(&server_address, 0, sizeof(server_address));

	// we set the address family to AF_INET (IPv4)
	server_address.sin_family = AF_INET;

	// we set the port number to the port number in the server config
	server_address.sin_port = htons(server.port[0]);

	// we set the IP address to host value in the server config
	server_address.sin_addr.s_addr = htonl(server.host);

	// we bind the socket to the server address using bind with the following parameters:
	// socket_fd: the socket file descriptor
	// (struct sockaddr *)&server_address: the server address cast to a sockaddr struct
	// sizeof(server_address): the size of the server address
	int result = bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));

	// if the result is less than 0, we throw an exception
	if (result < 0)
	{
		throw std::runtime_error("failed to bind socket to server address");
	}
}

void Server::set_socket_to_listen(int socket_fd)
{
	// here we set the socket to listen for incoming connections
	// we use the listen function with the following parameters:
	// socket_fd: the socket file descriptor
	// 128: the maximum number of connections that can be queued
	int result = listen(socket_fd, 128);

	// if the result is less than 0, we throw an exception
	if (result < 0)
	{
		throw std::runtime_error("failed to set socket to listen");
	}
}

void Server::server_loop()
{
	while (true)
	{
		// we set the reads fd_set to be the same as the master fd_set
		this->reads = this->master;

		// we use the select function to check if there is any activity on the sockets with the following parameters:
		// this->max_fd + 1: the maximum file descriptor + 1 because file descriptors start from 0
		// &this->reads: the read fd_set
		// &this->writes: the write fd_set
		// NULL: the exception fd_set (we don't use it)
		// tv: the timeout value
		int result = select(this->max_fd + 1, &this->reads, &this->writes, NULL, &tv);

		// if the result is less than 0, we throw an exception
		if (result < 0)
			throw std::runtime_error("select failed");

		// we loop through the server fds
		for (size_t i = 0; i < this->server_fds.size(); i++)
		{
			// we check if the socket is in the reads fd_set
			// if so, it means that there is an incoming connection
			// so we accept the connection
			if (FD_ISSET(this->server_fds[i], &this->reads))
			{
				// we accept the incoming connection
				accept_connection(this->server_fds[i], i);
			}
		}

		// we loop through all the clients
		for (size_t i = 0; i < this->clients.size(); i++)
		{
			// we get the socket file descriptor of the client for easier access
			int fd = this->clients[i].socket_fd;

			// check for timeout
			check_for_timeout(this->clients[i], i);

			// we check if the socket is in the reads fd_set
			// if so, it means that there is data to be read
			// so we read the data
			if (FD_ISSET(fd, &this->reads))
				read_from_client(fd, i);

			// we check if the socket is in the writes fd_set
			// if so, it means that the socket is ready to be written to
			// so we write to the socket
			if (FD_ISSET(fd, &this->writes))
				write_to_client(fd, i);
		}
	}
}

void Server::accept_connection(int socket_fd, int index)
{
	// here we accept the incoming connection
	// we need to create a sockaddr_in struct to store the client address
	struct sockaddr_in client_address;

	// we create an integer to store the size of the client address
	socklen_t client_address_size = sizeof(client_address);

	// we accept the connection using accept with the following parameters:
	// socket_fd: the socket file descriptor
	// (struct sockaddr *)&client_address: the client address cast to a sockaddr struct
	// &client_address_size: the size of the client address
	int client_socket_fd = accept(socket_fd, (struct sockaddr *)&client_address, &client_address_size);
	
	// if the client_socket_fd is less than 0, we throw an exception
	if (client_socket_fd < 0)
	{
		throw std::runtime_error("failed to accept connection");
	}

	
	// we set the client socket to non-blocking
	set_socket_to_non_blocking(client_socket_fd);

	// we add the client socket to the master fd_set
	FD_SET(client_socket_fd, &this->master);

	// we create a Client object with the client_socket_fd, client address and port and the server config that the client is connected to
	// we also set the env variable of the client to the env variable of the server
	// and add it to the clients vector
	Client client(client_socket_fd, ntohl(client_address.sin_addr.s_addr), ntohs(client_address.sin_port), this->config.servers[index]);
	// we want to pass the environment variables to the client but each client must have its own copy
	client.env = std::vector<std::string>(this->env);
	this->clients.push_back(client);

	// we check if the client_socket_fd is greater than the max_fd
	// if so, we set the max_fd to the client_socket_fd
	if (client_socket_fd > this->max_fd)
		this->max_fd = client_socket_fd;

	// we set the timeout for client
	client.keep_alive_timeout = std::time(NULL);

	// we log that a connection has been accepted
	log("New Connection From: " + int_to_ip(ntohl(client_address.sin_addr.s_addr)) + " Assigned to Socket: " + itoa(client_socket_fd), GREEN);
}

void Server::read_from_client(int client_fd, int index)
{
	// here we read data from the client in chunks

	// we need to create a buffer to store the data
	char buffer[BUFFER_SIZE];

	// we clear the buffer using memset
	std::memset(buffer, 0, BUFFER_SIZE);

	// we read the data using recv with the following parameters:
	// client_fd: the socket file descriptor
	// buffer: the buffer to store the data
	// BUFFER_SIZE: the size of the buffer
	// 0: flags (default)
	int bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);

	// if the bytes_read is less than 0, it means that there was an error
	// so we log the error then we close the connection and return -1
	if (bytes_read < 0)
	{
		log("Failed to read from socket " + itoa(client_fd), RED);
		close_connection(client_fd, index);
		return;
	}

	// if the bytes_read is 0, it means that the client has closed the connection
	// so we log that the connection has been closed then we close the connection and return -1
	if (bytes_read == 0)
	{
		log("Connection On Socket " + itoa(client_fd) + " Closed By Client, Closing Connection ...", GREEN);
		close_connection(client_fd, index);
		return;
	}

	// we add the data to the request of the client


	if(this->clients[index].write_to_file == false)
		this->clients[index].request += std::string(buffer, bytes_read);
	else
	{
		write(this->clients[index].request_fd, buffer, bytes_read);
	}



	if (this->clients[index].write_to_file == false && this->clients[index].request.find("\r\n\r\n") != std::string::npos)
	{
		this->clients[index].write_to_file = true;

		size_t pos = this->clients[index].request.find("\r\n\r\n");

		std::string body = this->clients[index].request.substr(pos + 4);
		this->clients[index].request =  this->clients[index].request.substr(0, pos + 4);
		std::cout << "FOUND: |" << body << "|" << std::endl;
		write(this->clients[index].request_fd, body.c_str(), body.size());
	}

	// here we check if the request is complete by checking if MSG_PEEK returns less than 0
	// if it returns less than 0, it means that there is no more data to be read
	if(bytes_read < BUFFER_SIZE)
	{
		this->clients[index].write_to_file = false;

		// we process the request
		// this function will parse and process the request then generate a response
		this->clients[index].handle_request();

		// we add the socket to the writes fd_set
		FD_SET(client_fd, &this->writes);
	}

	// we update the timeout of the client
	this->clients[index].keep_alive_timeout = std::time(NULL);
}

void Server::write_to_client(int socket_fd, int index)
{
	// here we write data to the client in chunks
	
	// we get the response of the client for easier access
	std::string response = this->clients[index].response;

	// we get the size of the response
	size_t response_size = response.size();

	// we get the size of the response that has been sent
	size_t sent_size = this->clients[index].sent_size;

	// we get the remaining size of the response
	size_t remaining_size = response_size - sent_size;

	// we get the size of the data to be sent
	size_t send_size = remaining_size > BUFFER_SIZE ? BUFFER_SIZE : remaining_size;

	// we get the data to be sent
	std::string data = response.substr(sent_size, send_size);

	// we convert the data to a c string
	const char *buffer = data.c_str();

	// we write the data using send with the following parameters:
	// socket_fd: the socket file descriptor
	// buffer: the buffer to store the data
	// send_size: the size of the data
	// 0: flags (default)
	int bytes_sent = send(socket_fd, buffer, send_size, 0);

	// if the bytes_sent is less than 0, it means that there was an error
	// so we log the error then we close the connection and return -1
	if (bytes_sent < 0)
	{
		log("Failed to write to socket " + itoa(socket_fd), RED);
		close_connection(socket_fd, index);
		return;
	}

	// we add the bytes_sent to the sent_size
	this->clients[index].sent_size += bytes_sent;

	// we check if the sent_size is equal to the response_size
	// this will mean that we have sent all the data
	if (this->clients[index].sent_size == response_size)
	{
		// log the response
    	log("Response Sent To: " + clients[index].request_host + ", Status Code: " + itoa(clients[index].response_status_code) + " " + clients[index].status_codes[clients[index].response_status_code], CYAN);
		
		// we clear the request and response of the client
		this->clients[index].request.clear();
		this->clients[index].response.clear();

		// we set the sent_size to 0
		// this will reset the sent_size for the next request
		this->clients[index].sent_size = 0;

		// we check if the connection is close
		// it means that the client wants to close the connection
		if(this->clients[index].connection == "close")
		{
			// we log that the connection has been closed
			log("Connection On Socket " + itoa(socket_fd) + " Closed By Server, Closing Connection ...", GREEN);

			// we close the connection
			close_connection(socket_fd, index);
		}
		
		// else we remove the socket from the writes fd_set
		// because we have sent all the data
		// and we dont close the connection because the client wants to keep the connection open
		else
			FD_CLR(socket_fd, &this->writes);
	}

	// we update the timeout of the client
	this->clients[index].keep_alive_timeout = std::time(NULL);
}

void Server::check_for_timeout(Client& client, int index)
{
	// here we check for timeout
	// if the client has been inactive for more than the timeout value, we close the connection
	
	// we get the current time
	time_t current_time = std::time(NULL);

	// we check if the difference is greater than the timeout value
	if (current_time - client.keep_alive_timeout >= REQUEST_TIMEOUT)
	{
		// we log that the connection has timed out
		log("Connection On Socket " + itoa(client.socket_fd) + " Timed Out, Closing Connection ...", GREEN);

		// we close the connection
		close_connection(client.socket_fd, index);

		return;
	}
}

void Server::close_connection(int socket_fd, int index)
{
	// here we close the connection

	//remove the socket from the master and writes fd_set
	FD_CLR(socket_fd, &this->master);
	FD_CLR(socket_fd, &this->writes);

	// we use the close function with the socket_fd as the parameter
	close(socket_fd);

	// we remove the client from the clients vector
	this->clients.erase(this->clients.begin() + index);

}