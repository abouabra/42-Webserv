#include "../includes/Client.hpp"

Client::Client() {
}

Client::Client(int socket_fd, int host, int port, ServerConfig config) {
    this->socket_fd = socket_fd;
    this->host = host;
    this->port = port;
    this->config = config;
    this->sent_size = 0;
    this->timeout = time(NULL);
}

Client::~Client() {
}

Client &Client::operator=(Client const &obj)
{
    if (this != &obj) {
        this->socket_fd = obj.socket_fd;
        this->host = obj.host;
        this->port = obj.port;
        this->request = obj.request;
        this->response = obj.response;
        this->config = obj.config;
        this->sent_size = obj.sent_size;
        this->timeout = obj.timeout;
    }
    return *this;
}

void Client::process_request() {
    this->response = "HTTP/1.1 200 OK\r\n";
    this->response += "Content-Type: text/html\r\n";
    this->response += "Content-Length: 12\r\n";
    this->response += "Connection: keep-alive\r\n";
    this->response += "\r\n";
    this->response += "Hello World!";
    this->sent_size = 0;
}