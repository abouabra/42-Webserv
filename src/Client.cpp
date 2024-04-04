#include "../includes/Client.hpp"
#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

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
		process_GET(config.locations[location_idx]);
	else if (method == "POST")
		process_POST(config.locations[location_idx]);
	else if (method == "DELETE")
		process_DELETE(config.locations[location_idx]);
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
		.set_content_type("text/html")
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

void Client::process_GET(Location& location)
{
	// here we process the GET request

	// we get the full path of the resource
	std::string full_path = construct_resource_path(location);

	// we check if the resource exists
	// if it fails send a 404 response and return
	if (verify_resource_existence(full_path) == false)
		return;

	// we check if the resource is a directory or a file
	if(is_path_directory(full_path) == true)
		process_directory(location, full_path);
	else
		process_file(full_path);

}

std::string Client::construct_resource_path(Location& location)
{
	// here we get the full path of the resource

	// we define the root path
	// if the location root is empty we fallback to the server root
	std::string root = location.root.empty() ? this->config.root : location.root;

	// we define the URL path
	// we need to remove the first character from the uri because forward slash already exists in the root path
	std::string url_path = uri.substr(1);

	// we define the full path
	// we do this by concatenating the root path and the uri
	std::string full_path = root + url_path;

	// we return the full path
	return full_path;
}

bool Client::verify_resource_existence(std::string &resource_path)
{
	// here we check if the resource exists
	// if it does we return true otherwise we send a 404 response

	// we check if the resource exists using the access system call
	// it needs the resource path and the F_OK flag to check if the file exists
	// F_OK is a flag that checks if the file exists
	if (access(resource_path.c_str(), F_OK) == -1)
	{
		send_error_response(404);
		return false;
	}

	// if the resource exists we return true
	return true;
}

bool Client::is_path_directory(std::string &path)
{
	// here we check if the resource is a directory

	// we define a struct to hold the file information
	struct stat s;

	// we use the stat system call to get the file information
	stat(path.c_str(),&s);

	// The s struct, holds a bitmask representing various file attributes.
	// we do the bitwise AND operator (&) to isolate the specific bit that indicates directory status using the S_IFDIR flag.
	// If the resulting value is non-zero, it confirms that the file is a directory and returns true.
	if(s.st_mode & S_IFDIR)
		return true;

	// otherwise we return false
	return false;
}

void Client::process_directory(Location& location, std::string full_path)
{
	// here we process the directory

	// we check if the uri has a doesnt have a trailing slash
	// if so we redirect to the uri with a trailing slash
	if(has_uri_trailing_slash() == false)
		return;

	// we check if the index file is defined in the location
	// and if it exists in the directory
	// if all requirements are met we serve the index file
	if(location.index.empty() == false && access((full_path + location.index).c_str(), F_OK) != -1)
	{
		// process the file
		process_file(full_path + location.index);
		return;
	}

	// here we check if the directory listing is enabled
	// if true we serve the directory listing
	if(location.directory_listing == true)
	{
		serve_directory_listing(full_path);
		return;
	}

	// if we reach here we send a 403 response
	send_error_response(403);
}

bool Client::has_uri_trailing_slash()
{
	// here we check if the uri has a trailing slash
	// if it does we send a 301 response

	// we check if the uri doesnt a trailing slash
	// we do this by checking the last character of the uri
	if (uri[uri.size() - 1] != '/')
	{
		// we build the raw response for the redirection

		// we define the status line
		this->response = "HTTP/1.1 301 Moved Permanently\r\n";

		// we define the Location header that contains the redirection URL
		this->response += "Location: " + this->uri + "/\r\n";

		// we define the connection header
		this->response += "Connection: " + this->connection + "\r\n";

		// we define the content length header
		this->response += "Content-Length: 0\r\n";

		// we define the content type header
		this->response += "Content-Type: text/html\r\n";

		// we end the headers with a carriage return and line feed (\r\n)
		this->response += "\r\n";

		// we return false to indicate that the uri has a no trailing slash
		return false;
	}

	// we return true to indicate that the uri does have a trailing slash
	return true;
}

void Client::serve_directory_listing(std::string &resource_path)
{
	// This function opens the directory at the specified resource path, iterates through
	// its entries, and builds an HTML response containing a table with file information
	// (name, size, last modified date). It also handles parent directory navigation
	// and includes icons for different file types (using the `extension_to_html_icon` function).

	// we define the html header with css
	std::string body = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Index of " + resource_path + "</title><style>";
	body+= "body{font-family:monospace;font-size:16px;margin:0;padding:20px;background-color:#141615;color:#fff}.table_container{display:flex;flex-direction:column;justify-content:center;align-items:center}h1{margin-bottom:20px}table{width:50%;border-collapse:collapse}th,td{padding:5px 10px;border:1px solid transparent}th{text-align:left;font-weight:bold;font-size:18px}th:nth-child(1){width:50%}a{color:#fff;text-decoration:none}a:hover{text-decoration:underline}table tbody tr:hover{background-color:#3b3b3b}";
	body+= "</style></head><body><div class=\"table_container\"><h1>Index of "+ resource_path +"</h1><table><thead><tr><th>Name</th><th>Size</th><th>Last Modified</th></tr></thead><tbody>";

	// we define variables
	struct dirent *entry;
	std::vector<std::string> files;
	std::string line;
	char time_str[80];
	DIR *dp;

	// we open the directory
	dp = opendir(resource_path.c_str());

	// Process directory entries
	// we loop through the directory entries
	while ((entry = readdir(dp)))
	{
		// we get the full path of the entry
		std::string full_path = resource_path + "/" + entry->d_name;

		// we get the file information
		struct stat file_stat;
		stat(full_path.c_str(), &file_stat);

		// if the entry is a file we get the file information
		if (entry->d_type == DT_REG)
		{
			// we get the file size then convert it to human readable format
			std::time_t mtime = file_stat.st_mtime;
			std::tm* mod_time = std::localtime(&mtime);
			strftime(time_str, sizeof(time_str), "%d/%m/%Y %H:%M:%S", mod_time);

			// we get the file extension
			std::string extension = full_path.substr(full_path.find_last_of(".") + 1);
			
			// we build the line for the file
			line = "<tr><td>" + extension_to_html_icon(extension) + "; <a href=\"" + std::string(entry->d_name) + "\">" + std::string(entry->d_name) + "</a></td><td>" + convert_size(file_stat.st_size) + "</td><td>" + time_str + "</td></tr>";
		}
		// if the entry is a directory we get the directory information
		else
		{
			// we skip the current directory
			if(entry->d_name == std::string("."))
				continue;

			// we have a custom line for the parent directory
			// we have to continue to skip the parent directory from being added to files to not get sorted cause then it will not apear first
			if(entry->d_name == std::string(".."))
			{
				line = "<tr><td>&#128281; <a href=\"../\">Parent Directory</a></td><td>&nbsp;</td><td>&nbsp;</td></tr>";
				body+= line;
				continue;
			}

			// we build the line for the directory
			else
				line = "<tr><td>&#128193; <a href=\"" + std::string(entry->d_name) + "/\">" + std::string(entry->d_name) + "/</a></td><td>&nbsp;</td><td>&nbsp;</td></tr>";
		}

		// we add the line to the files vector
		files.push_back(line);
	}

	// we close the directory
	closedir(dp);

	// we sort the files in alphabetical order
	std::sort(files.begin(), files.end());

	// we add the files to the body
	for (size_t i = 0; i < files.size(); i++)
		body += files[i];

	// we end the body
	body += "</tbody></table></div></body></html>";

	// we build the response
	this->set_status_code(200)
		.set_content_type("text/html")
		.set_connection(this->connection)
		.set_body(body)
		.build_response();
}

void Client::process_file(std::string resource_path)
{
	// check if the file is static file or dynamic file
	if(should_be_processed_by_cgi(resource_path) == true)
		serve_dynamic_content(resource_path);
	else
		serve_static_content(resource_path);
}

bool Client::should_be_processed_by_cgi(std::string &resource_path)
{
	// here we check if the file is a dynamic file
	// we do this by checking the file extension

	// we get the file extension
	std::string extension = resource_path.substr(resource_path.find_last_of(".") + 1);

	// we check if the file extension is in cgi map in server block
	for (std::map<std::string, std::string>::iterator it = this->config.cgi.begin(); it != this->config.cgi.end(); it++)
	{
		// if the file extension is in the cgi map we return true
		if (it->first == extension)
			return true;
	}

	// otherwise we return false
	return false;
}

void Client::serve_static_content(std::string &resource_path)
{
	// here we serve the static content

	// we get the file extension
	std::string extension = resource_path.substr(resource_path.find_last_of(".") + 1);
	if(extension.empty())
		extension = "txt";
	// we read the file and send it as the response
	this->set_status_code(200)
		.set_content_type(mime_types[extension])
		.set_connection(connection)
		.set_body(read_file(resource_path))
		.build_response();
}

void Client::serve_dynamic_content(std::string &resource_path)
{
	(void) resource_path;

    // we send a test response
    this->set_status_code(200)
        .set_content_type("text/html")
        .set_connection(this->connection)
        .set_body("Hello from GET")
        .build_response();
}

void Client::process_POST(Location& location)
{
	(void) location;



	// we send a test response
    this->set_status_code(200)
        .set_content_type("text/html")
        .set_connection(this->connection)
        .set_body("Hello from POST")
        .build_response();
}

void Client::process_DELETE(Location& location)
{
	(void) location;




	// we send a test response
    this->set_status_code(200)
        .set_content_type("text/html")
        .set_connection(this->connection)
        .set_body("Hello from DELETE")
        .build_response();
}