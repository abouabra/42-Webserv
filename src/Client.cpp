#include "../includes/Client.hpp"
#include <sstream>

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


Client& Client::set_status_code(int status_code) {
    this->response_status_code = status_code;
    return *this;
}

Client& Client::set_reason_phrase(std::string reason_phrase) {
    this->response_reason_phrase = reason_phrase;
    return *this;
}

Client& Client::set_content_type(std::string content_type) {
    this->response_content_type = content_type;
    return *this;
}

Client& Client::set_connection(std::string connection) {
    this->response_connection = connection;
    return *this;
}

Client& Client::set_body(std::string body) {
    this->response_body = body;
    return *this;
}

void Client::build_response() {
    this->response = "HTTP/1.1 " + itoa(this->response_status_code) + " " + this->response_reason_phrase + "\r\n";
    this->response += "Content-Type: " + this->response_content_type + "\r\n";
    this->response += "Connection: " + this->response_connection + "\r\n";
    this->response += "Content-Length: " + itoa(this->response_body.size()) + "\r\n";
    this->response += "\r\n";
    this->response += this->response_body;
}

void Client::handle_request() {

    // parse and extract request parameters
    parse_request();

    // process the request and generate response
    process_request();

}
void Client::parse_request()
{
    std::stringstream ss(request);
    std::string line;

    // here we loop through the request line by line
    // and parse the request header
    while(std::getline(ss, line)) {

        //replace \r with \0 to remove the carriage return
        line.replace(line.find("\r"), 1, "\0");

        // check if we reached the end of request header
        if (line.empty())
            break;

        // here we statrt to parse request headers
        // we check for each header and extract the value


        // we parse method, uri and protocol
        if (this->method.empty()) {
            std::stringstream ss_2(line);
            ss_2 >> this->method >> this->uri >> this->protocol;
        }


        // we parse the host header
        if (line.find("Host: ") != std::string::npos)
            this->request_host = line.substr(6);

        // we parse the connection header
        if (line.find("Connection: ") != std::string::npos)
            this->connection = line.substr(12);

        // we parse the content-length header
        if (line.find("Content-Length: ") != std::string::npos)
            this->content_length = line.substr(16);

        // we parse the content-type header
        if (line.find("Content-Type: ") != std::string::npos)
            this->content_type = line.substr(14);

        // we parse the cookie header
        if (line.find("Cookie: ") != std::string::npos)
            this->cookie = line.substr(8);
    }

    // parse request body    
    std::getline(ss, this->request_body, '\0');


    // we log that we have parsed the request
    log("Request Received From: " + this->request_host + ", Method: " + this->method + ", URI: " + this->uri, CYAN);


    // print parsed request parameters
    // std::cout << "Method: |" << this->method << "|" << std::endl;
    // std::cout << "URI: |" << this->uri << "|" << std::endl;
    // std::cout << "Protocol: |" << this->protocol << "|" << std::endl;
    // std::cout << "Host: |" << this->request_host << "|" << std::endl;
    // std::cout << "Connection: |" << this->connection << "|" << std::endl;
    // std::cout << "Content-Length: |" << this->content_length << "|" << std::endl;
    // std::cout << "Content-Type: |" << this->content_type << "|" << std::endl;
    // std::cout << "Cookie: |" << this->cookie << "|" << std::endl;
    // std::cout << std::endl;
    // std::cout << "Request Body: |" << this->request_body << "|" << std::endl;

}

void Client::process_request() {
    // here we process the request and generate the response



    // we send a test response
    this->set_status_code(200)
        .set_reason_phrase("OK")
        .set_content_type("text/html")
        .set_connection("close")
        .set_body("Hello World")
        .build_response();
    
    // log the response
    log("Response Sent To: " + this->request_host + ", Status Code: " + itoa(this->response_status_code) + " " + this->response_reason_phrase, CYAN);

}