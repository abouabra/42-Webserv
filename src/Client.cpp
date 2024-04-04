#include "../includes/Client.hpp"
#include <sstream>
#include <string>

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
	this->status_codes[200] = "OK";
    this->status_codes[201] = "Created";
    this->status_codes[204] = "No Content";
    this->status_codes[301] = "Moved Permanently";
    this->status_codes[400] = "Bad Request";
    this->status_codes[401] = "Unauthorized";
    this->status_codes[403] = "Forbidden";
    this->status_codes[404] = "Not Found";
    this->status_codes[405] = "Method Not Allowed";
    this->status_codes[410] = "Gone";
    this->status_codes[413] = "Payload Too Large";
    this->status_codes[414] = "URI Too Long";
    this->status_codes[500] = "Internal Server Error";
    this->status_codes[501] = "Not Implemented";
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

void Client::handle_request()
{
    // parse and extract request parameters
    parse_request();

    // process the request and generate response
    process_request();

	// log the response
    log("Response Sent To: " + this->request_host + ", Status Code: " + itoa(this->response_status_code) + " " + status_codes[this->response_status_code], CYAN);
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

    // here we have multiple checks if the request is valid
    // if not we send a 4xx response
	if(check_request_validity() == false)
		return;

	// here we check if the URI is matched with a location
	// if it did return -1 we send a 404 response
	int location_idx = find_matching_location();
	if(location_idx == -1)
		return;

	// here we check if location is has a redirect
	// if it does we send a 301 response
	if(process_location_redirection(location_idx) == true)
		return;

	// here we check if the method is allowed in the location
	// if not we send a 405 response
	if(validate_method_for_location(location_idx) == false)
		return;

	// check what method is used and call the appropriate function
	if (method == "GET")
		process_GET(location_idx);
	else if (method == "POST")
		process_POST(location_idx);
	else if (method == "DELETE")
		process_DELETE(location_idx);
}

bool Client::check_request_validity()
{
    // check if the uri contains any invalid characters or empty
	// if it does we send a 400 response
    std::string URI_charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	if (uri.find_first_not_of(URI_charset) != std::string::npos || uri.empty())
	{
	    send_error_response(400);
	    return false;
	}

	// check if uri size is too long
	// if it is we send a 414 response
	if (uri.size() > 2048)
	{
	    send_error_response(414);
	    return false;
	}

	// check body size larger than max body size
	// if it is we send a 413 response
	if (request_body.size() > config.max_body_size)
	{
	    send_error_response(413);
	    return false;
	}

	// check if the method is implemented in the server
	// if not we send a 501 response
	if(method != "GET" && method != "POST" && method != "DELETE")
	{
		send_error_response(501);
		return false;
	}

	// if we reach here the request is well formed
    return true;
}


// Helper function to send error response with code and body retrieval
void Client::send_error_response(int status_code) {
    this->set_status_code(status_code)
        .set_connection(this->connection)
        .set_body(check_error_page(status_code))
        .build_response();
}


std::string Client::check_error_page(int status_code)
{
	// here we check if the status code is defined in server block in config file
    // if found we send it otherwise we send a generic error page

	// we check if the error page is defined in the config file
    if (this->config.error_pages.find(status_code) != this->config.error_pages.end())
    {
		//here we define the path to the error page
		// it is the root path + the error page path
        std::string error_page_path = config.root + this->config.error_pages[status_code];

        // we try to read the error page
        std::string content = read_file(error_page_path);

		// if the error page is not found we send a generic error page
		if (content.empty())
			return generic_error_page(status_code);

		//otherwise we return the content of the error page
        return content;
    }

	// if the error page is not defined we send a generic error page
	return generic_error_page(status_code);
}

std::string Client::generic_error_page(int status_code)
{
	// we define a generic error page
	// this generic error page will be sent if the error page is not defined in the config file
	// or if the server cant find the error page

	// defined the html header
	std::string generic_error_page = "<html><head><title>" + itoa(status_code) + " " + status_codes[status_code] + "</title>";
	
	//here we define the css for the error page
	generic_error_page += "<style>body {background-color: #141615; color: #fff; margin: 0; padding: 0;} h1 {text-align: center;}</style> </head>";
	
	// here we define the body of the error page
	generic_error_page += "<body><h1>" + itoa(status_code) + " " + status_codes[status_code] + "</h1></body></html>";

	return generic_error_page;
}

int Client::find_matching_location()
{
	// here we try to get the matched location for the request uri
	// we will use the Longest Prefix Match algorithm to get the matched location
	// it is the same algorithm used in Nginx

	// here we define the matched index and length
	// we will set matched index to -1 as a default value to indicate no location matched
	int matched_index = -1;
	size_t matched_length = 0;

	// here we loop through the locations in the server block in the config file
	for (size_t i = 0; i < this->config.locations.size(); i++)
	{
		// here we get the location path for easier access
		std::string location = this->config.locations[i].path;

		// The Longest Prefix Match this is done by:
		// checking if the location path is a "prefix" of the request URI thats why we compare result of find with 0
		// checking if the location path is longer than the previous matched location
		// if both conditions are met we update the matched index and length
		if (uri.find(location) == 0 && location.size() > matched_length)
		{
			matched_index = i;
			matched_length = location.size();
		}
	}

	// if no location matched we send a 404 response
	if(matched_index == -1)
	{
		send_error_response(404);
		return -1;
	}

	// we return the matched index
	return matched_index;
}

bool Client::process_location_redirection(int location_idx)
{
	// here we check if the location has a redirection
	// if it does we send a 301 response

	// here we check if the location has a redirection
	// we do this by checking if the redirection URL is not empty
	if (this->config.locations[location_idx].redirect_URL.empty() == false)
	{
		// here we build the raw response for the redirection

		// we define the status line
		this->response = "HTTP/1.1 301 Moved Permanently\r\n";

		// we define the Location header that contains the redirection URL
		this->response += "Location: " + this->config.locations[location_idx].redirect_URL + "\r\n";

		// we define the connection header
		this->response += "Connection: " + this->connection + "\r\n";

		// we define the content length header
		this->response += "Content-Length: 0\r\n";

		// we define the content type header
		this->response += "Content-Type: text/html\r\n";

		// we end the headers with a carriage return and line feed (\r\n)
		this->response += "\r\n";

		// we return true to indicate that the location has a redirection
		return true;
	}

	// we return false to indicate that the location does not have a redirection
	return false;
}

bool Client::validate_method_for_location(int location_idx)
{
	// here we check if the method is allowed in the location
	// if not we send a 405 response

	// here we loop through the allowed methods in the location
	// and check if the method is allowed
	for (size_t i = 0; i < this->config.locations[location_idx].methods.size(); i++)
	{
		// if the method is in the allowed methods we return true
		if (this->config.locations[location_idx].methods[i] == this->method)
			return true;
	}

	// if the method is not allowed we send a 405 response
	send_error_response(405);
	return false;
}

void Client::process_GET(int location_idx)
{
	(void) location_idx;

    // we send a test response
    this->set_status_code(200)
        .set_content_type("text/html")
        .set_connection(this->connection)
        .set_body("Hello from GET")
        .build_response();
}

void Client::process_POST(int location_idx)
{
	(void) location_idx;



	// we send a test response
    this->set_status_code(200)
        .set_content_type("text/html")
        .set_connection(this->connection)
        .set_body("Hello from POST")
        .build_response();
}

void Client::process_DELETE(int location_idx)
{
	(void) location_idx;




	// we send a test response
    this->set_status_code(200)
        .set_content_type("text/html")
        .set_connection(this->connection)
        .set_body("Hello from DELETE")
        .build_response();
}