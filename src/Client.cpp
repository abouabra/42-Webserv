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

    // initialize status codes
    this->status_codes[201] = "Created";
    this->status_codes[204] = "No Content";
    this->status_codes[301] = "Moved Permanently";
    this->status_codes[400] = "Bad Request";
    this->status_codes[401] = "Unauthorized";
    this->status_codes[403] = "Forbidden";
    this->status_codes[404] = "Not Found";
    this->status_codes[405] = "Method Not Allowed";
    this->status_codes[410] = "Gone";
    this->status_codes[500] = "Internal Server Error";
    this->status_codes[502] = "Bad Gateway";
    this->status_codes[503] = "Service Unavailable";


    // initialize mime types
    this->mime_types["html"] = "text/html";
    this->mime_types["css"] = "text/css";
    this->mime_types["js"] = "text/javascript";
    this->mime_types["jpg"] = "image/jpeg";
    this->mime_types["jpeg"] = "image/jpeg";
    this->mime_types["png"] = "image/png";
    this->mime_types["gif"] = "image/gif";
    this->mime_types["ico"] = "image/x-icon";
    this->mime_types["svg"] = "image/svg+xml";
    this->mime_types["mp3"] = "audio/mpeg";
    this->mime_types["ogg"] = "audio/ogg";
    this->mime_types["wav"] = "audio/vnd.wave";
    this->mime_types["mp4"] = "video/mp4";
    this->mime_types["webm"] = "video/webm";
    this->mime_types["ogg"] = "video/ogg";
    this->mime_types["pdf"] = "application/pdf";
    this->mime_types["doc"] = "application/msword";
    this->mime_types["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    this->mime_types["xls"] = "application/vnd.ms-excel";
    this->mime_types["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    this->mime_types["ppt"] = "application/vnd.ms-powerpoint";
    this->mime_types["zip"] = "application/zip";
    this->mime_types["gz"] = "application/gzip";
    this->mime_types["tar"] = "application/x-tar";
    this->mime_types["ttf"] = "font/ttf";
    this->mime_types["otf"] = "font/otf";
    this->mime_types["txt"] = "text/plain";
    this->mime_types["json"] = "application/json";

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

        this->method = obj.method;
        this->uri = obj.uri;
        this->protocol = obj.protocol;
        this->request_host = obj.request_host;
        this->connection = obj.connection;
        this->content_length = obj.content_length;
        this->content_type = obj.content_type;
        this->cookie = obj.cookie;
        this->request_body = obj.request_body;

        this->response_status_code = obj.response_status_code;
        this->response_connection = obj.response_connection;
        this->response_body = obj.response_body;

        this->status_codes = obj.status_codes;
        this->mime_types = obj.mime_types;
    }
    return *this;
}


Client& Client::set_status_code(int status_code) {
    this->response_status_code = status_code;
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

Client& Client::set_content_type(std::string content_type) {
    this->response_content_type = content_type;
    return *this;
}

void Client::build_response() {
    this->response = "HTTP/1.1 " + itoa(this->response_status_code) + " " + status_codes[response_status_code] + "\r\n";
    this->response += "Connection: " + this->response_connection + "\r\n";
    this->response += "Content-Length: " + itoa(this->response_body.size()) + "\r\n";
    this->response += "Content-Type: " + this->response_content_type + "\r\n";
    this->response += "\r\n";
    this->response += this->response_body;
}

void Client::handle_request() {

    // std::cout << this->request << std::endl;

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
        if (line.find("HTTP") != std::string::npos)
        {
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
        .set_content_type("text/html")
        .set_content_type("text/html")
        .set_connection(this->connection)
        .set_body("Hello World")
        .build_response();
    
    // log the response
    log("Response Sent To: " + this->request_host + ", Status Code: " + itoa(this->response_status_code) + " " + status_codes[response_status_code], CYAN);

}