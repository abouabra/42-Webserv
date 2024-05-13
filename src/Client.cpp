#include "../includes/Client.hpp"
#include <algorithm>
#include <signal.h>
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <iostream>
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
    this->keep_alive_timeout = std::time(NULL);

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
        this->keep_alive_timeout = obj.keep_alive_timeout;

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
    this->response += "Connection: " + this->connection + "\r\n";
    this->response += "Content-Length: " + itoa(this->response_body.size()) + "\r\n";
    this->response += "Content-Type: " + this->response_content_type + "\r\n";
    this->response += "\r\n";
    this->response += this->response_body;
}

void Client::handle_request()
{
    parse_request();

    // process the request and generate response
    process_request();

	// we check if the method is HEAD
	// if it is we remove the body from the response
	if(method == "HEAD")
	{
		// we find the position of the first \r\n\r\n
		size_t pos = response.find("\r\n\r\n");


		// we remove the body from the response
		// we do this by getting the substring from the beginning to the position of the first \r\n\r\n
		// we add 4 to the position to include the \r\n\r\n
		this->response = response.substr(0, pos + 4);
	}
}

void Client::parse_request()
{
    std::stringstream ss(request);
    std::string line;

	this->method.clear();
	this->uri.clear();
	this->protocol.clear();
	this->request_host.clear();
	this->connection.clear();
	this->content_length.clear();
	this->content_type.clear();
	this->transfer_encoding.clear();
	this->request_query_string.clear();
	this->cookie.clear();
   
    while(std::getline(ss, line)) {

        // remove carriage return from the line
        line = line.substr(0, line.size() - 1);

        // check if we reached the end of request header
        if (line.empty())
            break;

        // we parse method, uri and protocol
        if (line.find("HTTP") != std::string::npos)
        {
            std::stringstream ss_2(line);
            ss_2 >> this->method >> this->uri >> this->protocol;

			// parse query string if it exists (we found ?)
			size_t query_string_pos = this->uri.find("?");
			if(query_string_pos != std::string::npos)
			{
				// we extract the query string and remove it from the uri
				this->request_query_string = this->uri.substr(query_string_pos + 1);
				this->uri = this->uri.substr(0, query_string_pos);
			}

        }

        if (line.find("Host: ") != std::string::npos)
            this->request_host = line.substr(6);

        if (line.find("Connection: ") != std::string::npos)
            this->connection = line.substr(12);

        if (line.find("Content-Length: ") != std::string::npos)
            this->content_length = line.substr(16);

        if (line.find("Content-Type: ") != std::string::npos)
            this->content_type = line.substr(14);

		if (line.find("Transfer-Encoding: ") != std::string::npos)
			this->transfer_encoding = line.substr(19);

        if (line.find("Cookie: ") != std::string::npos)
            this->cookie = line.substr(8);
    }

	// parse request body
	this->request_body.clear();

	// we read the request body
	std::stringstream body_ss;
	body_ss << ss.rdbuf(); // Move what's inside the stream until EOF to body_ss
	this->request_body = body_ss.str();

	// we clear the request to make sure it is empty
	// so that if we have multiple requests in the same connection we dont have the previous request
	this->request.clear();

    log("Request Received From: " + this->request_host + ", Method: " + this->method + ", URI: " + decode_URL(uri), CYAN);

}

void Client::process_request() {
    // here we process the request and generate the response
	// std::cout << request << std::endl;

    // here we have multiple checks if the request is valid
    // if not we send a 4xx response
	if(check_request_validity() == false)
		return;
	
	// here we decode the uri after we have checked the request validity
	this->uri = decode_URL(this->uri);

	// here we check if the Transfer-Encoding header is chunked
	// if it is we decode the chunked body
	// and set the content length to the size of the decoded body
	if(transfer_encoding == "chunked")
	{
		decode_chunked_body();
		this->content_length = itoa(request_body.size());
	}

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
	if (method == "GET" || method == "POST" || method == "HEAD")
		process_GET_and_POST_and_HEAD(config.locations[location_idx]);
	else if (method == "DELETE")
		process_DELETE(config.locations[location_idx]);
}

bool Client::check_request_validity()
{
	// we check if transfer-Encoding header exist and is different to “chunked”
	// if it does we send a 501 response
	if (transfer_encoding.empty() == false && transfer_encoding != "chunked")
	{
		send_error_response(501);
		return false;
	}

	// if Transfer-Encoding does not exist and Content-Length does not exist and the method is POST
	// we send a 400 response
	if (transfer_encoding.empty() && content_length.empty() && method == "POST")
	{
		send_error_response(400);
		return false;
	}

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
	if(method != "GET" && method != "POST" && method != "DELETE" && method != "HEAD")
	{
		send_error_response(501);
		return false;
	}

	// if we reach here the request is well formed
    return true;
}

void Client::decode_chunked_body()
{
	// we do this by reading the chunked body and decoding it
	std::string decoded_body;

	std::stringstream ss(request_body);

	while(true)
	{
		// we define the chunk size
		size_t chunk_size;

		// we read the chunk size
		ss >> std::hex >> chunk_size;

		// we check if the chunk size is 0
		// if it is we break the loop
		if(chunk_size == 0 || ss.eof())
			break;
		
		// we ignore the \r\n from the chunk size
		ss.ignore(2);

		// we read the chunk data
		std::string chunk_data(chunk_size, '\0');
		ss.read(&chunk_data[0], chunk_size);

		// we remove the \r\n from the chunk data
		ss.ignore(2);

		// we add the chunk data to the decoded body
		decoded_body += chunk_data;
	}

	// we set the decoded body to the request body
	this->request_body = decoded_body;
}

// Helper function to send error response with code and body retrieval
void Client::send_error_response(int status_code) {
    this->set_status_code(status_code)
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
		try {
        	return read_file(error_page_path);
		}
		// if the error page is not found we send a generic error page
		catch(const std::exception& e) {
			return generic_error_page(status_code);
		}
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

void Client::process_GET_and_POST_and_HEAD(Location& location)
{
	// here we process the GET and POST and HEAD methods
	// they are processed in the same function because they have the same logic
	// they differ in CGI script execution

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
		process_file(full_path, location);

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
		process_file(full_path + location.index, location);
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
	body+= "body{font-family:monospace;font-size:16px;margin:0;padding:20px;background-color:#141615;color:#fff}.table_container{display:flex;flex-direction:column;justify-content:center;align-items:center}h1{margin-bottom:20px}table{width:50%;border-collapse:collapse}th,td{padding:5px 10px;border:1px solid transparent}th{text-align:left;font-weight:bold;font-size:18px}th:nth-child(1){width:70%}a{color:#fff;text-decoration:none}a:hover{text-decoration:underline}table tbody tr:hover{background-color:#3b3b3b}";
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
			std::strftime(time_str, sizeof(time_str), "%d/%m/%Y %H:%M:%S", mod_time);

			// we get the file extension
			std::string extension;
			try
			{
				extension = full_path.substr(full_path.find_last_of(".") + 1);
			}
			catch(const std::exception& e)
			{
				extension = "txt";
			}
			
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
		.set_body(body)
		.build_response();
}

void Client::process_file(std::string resource_path, Location& location)
{
	// check if the file is static file or dynamic file
	if(should_be_processed_by_cgi(resource_path) == true)
		serve_dynamic_content(resource_path, location);
	else
		serve_static_content(resource_path);
}

bool Client::should_be_processed_by_cgi(std::string &resource_path)
{
	// here we check if the file is a dynamic file
	// we do this by checking the file extension

	// we get the file extension
	std::string extension;
	try
	{
		extension = resource_path.substr(resource_path.find_last_of("."));

	}
	catch(const std::exception& e)
	{
		return false;
	}

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

	// we check if the method is POST
	// if it is we send a 405 response
	if(method == "POST")
	{
		send_error_response(405);
		return;
	}

	// we get the file extension
	std::string extension;
	try {
		extension = resource_path.substr(resource_path.find_last_of(".") + 1);
	} catch (std::exception &e) {
		extension = "txt";
	}
	
	std::string data;

	// we try to read the file
	try {
		data = read_file(resource_path);
	}
	// if page is not found we send a generic error page
	catch(const std::exception& e) {
		return send_error_response(404);
	}

	// we read the file and send it as the response
	this->set_status_code(200)
		.set_content_type(mime_types[extension])
		.set_body(data)
		.build_response();
}

void Client::serve_dynamic_content(std::string &resource_path, Location& location)
{
	// we check what method is used and call the appropriate function
	if(method == "GET" || method == "HEAD")
		process_GET_CGI(resource_path);
	else if (method == "POST")
		process_POST_CGI(resource_path, location);
	else if (method == "DELETE")
		process_DELETE_CGI(resource_path);
}

void Client::process_GET_CGI(std::string &resource_path)
{
	// here we run the CGI script for the GET method

	// we get the path of cgi-bin executable
	std::string executable_path = this->config.cgi[resource_path.substr(resource_path.find_last_of("."))].c_str();
	
	// we make a new vector to hold the environment variables
	std::vector<std::string> new_env;
	
	// we make new so that we can add the new environment variables and dont affect the original environment variables
	for (size_t i = 0; i < env.size(); i++)
		new_env.push_back(env[i]);

	// first we need to make environment variables QUERY_STRING and REQUEST_METHOD and PATH_INFO to pass to the CGI script
	new_env.push_back("QUERY_STRING=" + request_query_string);
	new_env.push_back("REQUEST_METHOD=GET");
	new_env.push_back("PATH_INFO=" + resource_path);
	
	// we need to add the cookie to the environment variables if it exists
	if(cookie.empty() == false)
	{
		std::stringstream ss(cookie);
		std::string item;
		while (std::getline(ss, item, ';'))
		{
			item = item[0] == ' ' ? item.substr(1) : item;
			new_env.push_back("HTTP_COOKIE_" + item);
		}
	}

	// we need to make a new char **argv
	// we need to add the executable path and the resource path
	char *argv[] = {my_strdup(executable_path), 
					my_strdup(resource_path),
					NULL};

	// we need to make a new char **envp
	char *envp[new_env.size() + 1];
	for (size_t i = 0; i < new_env.size(); i++)
		envp[i] = my_strdup(new_env[i]);
	envp[new_env.size()] = NULL;

	// we execute the CGI script
	execute_CGI(executable_path.c_str(), argv, envp);

	// we free the memory

	// we free the memory allocated for the environment variables
	for (size_t i = 0; i < new_env.size(); i++)
		delete envp[i];

	// we free the memory allocated for the argv
	for (size_t i = 0; i < 2; i++)
		delete argv[i];
}

void Client::execute_CGI(const char *path, char *argv[], char *envp[])
{

	int pipe_fd[2];
	int status;
	std::string filename;
	
	// we check if the method is POST
	// if it is we generate a unique file name
	if(method == "POST")
		filename = GenerateUniqueFileName();

	// we create the pipe
	if(pipe(pipe_fd) < 0)
	{
		// if the pipe fails we send a 500 response
		send_error_response(500);
		return;
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		// if the fork fails we close the pipes and then send a 500 response
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		send_error_response(500);
		return;
	}

	if (pid == 0)
	{
		// we close the read end of the pipe
		close(pipe_fd[0]);

		// we check if the method is POST
		// if we make the stdin the read end of the pipe
		// and we write the request body to the write end of the pipe
		if(method == "POST")
		{
			// we create a a temporary file to write the request body
			// this is done bacause writing to file is waaaay faster than writing to pipe
			int fd = open(filename.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
			if(fd < 0)
			{
				// if the file fails to open we send a 500 response
				send_error_response(500);
				std::exit(1);
			}

			// we write the request body to the file
			int bytes = write(fd, request_body.c_str(), request_body.size());

			// if the bytes written is less than 0 it means an error occurred
			// we send a 500 response, close the pipe and return
			if (bytes < 0)
			{
				send_error_response(500);
				close(pipe_fd[1]);
				close(fd);
				return;
			}

			// we close the file
			close(fd);

			// now we re open the file in read only mode
			fd = open(filename.c_str(), O_RDONLY);

			// if the file fails to open we send a 500 response
			if(fd < 0)
			{
				send_error_response(500);
				std::exit(1);
			}

			// we redirect the stdin to the read end of the fd
			dup2(fd, STDIN_FILENO);

			close(fd);
		}

		// we redirect the stdout to the write end of the pipe
		dup2(pipe_fd[1], STDOUT_FILENO);

		execve(path, argv, envp);

		// if the execve fails we send a 500 response
		send_error_response(500);
		std::exit(1);
	}

	else
	{
		// we close the write end of the pipe
		close(pipe_fd[1]);

		// we define out starting time
		time_t start_time = std::time(NULL);

		// here we check for timeout of the child process
		while(true)
		{
			// we wait for the child process to finish
			// we need to recover the exit status of the child process
			int result = waitpid(pid, &status, WNOHANG);

			// we check if the child process has finished
			// result will be the PID of the child process if it has finished
			if (result > 0)
				break;

			// we check if the child process has timed out
			time_t current_time = std::time(NULL);
			if (current_time - start_time >= CGI_TIMEOUT)
			{
				// if the child process has timed out we kill the child process
				kill(pid, SIGKILL);

				// we close the read end of the pipe
				close(pipe_fd[0]);

				// we send a 500 response
				send_error_response(500);
				return;
			}
		}

		//extract the exit status of the child process
		status = status >> 8;

		// we check if the CGI script failed
		if (status != 0)
		{
			// if the CGI script fails we send a 500 response
			send_error_response(500);
			return;
		}

		// we read the response from the CGI script

		// we define varibales
		char buffer[4096];
		std::memset(buffer, 0, 4096);
		std::string line;
		// we loop through the response from the CGI script
		while (true)
		{
			// we read the response from the CGI script
			int bytes_read = read(pipe_fd[0], buffer, 4096);

			// if the bytes read is 0 it means we reached the end of the response then we break the loop
			if (bytes_read == 0)
				break;

			// if the bytes read is less than 0 it means an error occurred
			// we send a 500 response, close the pipe and return
			if (bytes_read < 0)
			{
				send_error_response(500);
				close(pipe_fd[0]);
				return;
			}

			// we append the buffer to the line
			line += std::string(buffer, bytes_read);

			// we clear the buffer
			std::memset(buffer, 0, 4096);
		}

		// we close the read end of the pipe
		close(pipe_fd[0]);

		// we check if the method is POST
		// if it is we remove the temporary file
		if(method == "POST")
			std::remove(filename.c_str());
		

		// we set the raw received response from the CGI script as the response
		this->response_status_code = 200;
		this->response = line;
	}
}

void Client::process_POST_CGI(std::string &resource_path, Location& location)
{
	// here we run the CGI script for the POST method

	// we get the path of cgi-bin executable
	std::string executable_path = this->config.cgi[resource_path.substr(resource_path.find_last_of("."))].c_str();

	// we make a new vector to hold the environment variables
	std::vector<std::string> new_env;
	
	// we make new so that we can add the new environment variables and dont affect the original environment variables
	for (size_t i = 0; i < env.size(); i++)
		new_env.push_back(env[i]);

	// then we have to make environment variables REQUEST_METHOD and CONTENT_TYPE and CONTENT_LENGTH to pass to the CGI script
	new_env.push_back("REQUEST_METHOD=POST");
	new_env.push_back("CONTENT_TYPE=" + this->content_type);
	new_env.push_back("PATH_INFO=" + resource_path);
	new_env.push_back("CONTENT_LENGTH=" + this->content_length);
	new_env.push_back("UPLOAD_DIR=" + location.upload_dir);

	// we need to add the cookie to the environment variables if it exists
	if(cookie.empty() == false)
	{
		std::stringstream ss(cookie);
		std::string item;
		while (std::getline(ss, item, ';'))
		{
			item = item[0] == ' ' ? item.substr(1) : item;
			new_env.push_back("HTTP_COOKIE_" + item);
		}
	}

	// we need to make a new char **argv
	// we need to add the executable path and the resource path
	char *argv[] = {my_strdup(executable_path), 
					my_strdup(resource_path),
					NULL};


	// we need to make a new char **envp
	char *envp[new_env.size() + 1];
	for (size_t i = 0; i < new_env.size(); i++)
		envp[i] = my_strdup(new_env[i]);
	envp[new_env.size()] = NULL;

	// we execute the CGI script
	execute_CGI(executable_path.c_str(), argv, envp);

	// we free the memory

	// we free the memory allocated for the environment variables
	for (size_t i = 0; i < new_env.size(); i++)
		delete envp[i];

	// we free the memory allocated for the argv
	for (size_t i = 0; i < 2; i++)
		delete argv[i];

}

void Client::process_DELETE(Location& location)
{
	// here we process the DELETE method

	// we get the full path of the resource
	std::string full_path = construct_resource_path(location);

	// we check if the resource exists
	// if it fails send a 404 response and return
	if (verify_resource_existence(full_path) == false)
		return;

	// we check if the resource is a directory or a file
	if(is_path_directory(full_path) == true)
		process_directory_for_DELETE(location, full_path);
	else
		process_file_for_DELETE(full_path, location);
}

void Client::process_directory_for_DELETE(Location& location, std::string &full_path)
{
	// here we process the directory for the DELETE method

	// we check if the uri has a doesnt have a trailing slash
	// if so we send 400 conflict response
	if (uri[uri.size() - 1] != '/')
	{
		send_error_response(400);
		return;
	}

	// we check if the index file is defined in the location
	// and if it exists in the directory and if it is a dynamic file
	// if all requirements are met we serve the cgi index file
	// if not we send a 403 response
	if(location.index.empty() == false && access((full_path + location.index).c_str(), F_OK) != -1)
	{
		std::string new_path = full_path + location.index;
		if(should_be_processed_by_cgi(new_path) == true)
			serve_dynamic_content(new_path, location);
		else
			send_error_response(403);
		return;
	}

	// here we try to delete the directory
	if(recursive_deletion(full_path) != 0)
	{
		// if it fails we check why it failed
		// we check if its because write access on the folder
		// if it is we send a 500 response
		// otherwise we send a 403 response
		if(access(full_path.c_str(), W_OK) == 0)
			send_error_response(500);
		else
			send_error_response(403);
		return;
	}

	// if the folder is deleted we send a 204 response
	// because the folder is deleted and there is no content to send
	this->set_status_code(204)
		.set_body("")
		.build_response();
}

void Client::process_file_for_DELETE(std::string resource_path, Location& location)
{
	// here we process the file for the DELETE method
	// check if the file is static file or dynamic file
	if(should_be_processed_by_cgi(resource_path) == true)
		serve_dynamic_content(resource_path, location);
	else
		delete_file(resource_path);
}

void Client::process_DELETE_CGI(std::string &resource_path)
{
	// here we run the CGI script for the DELETE method

	// we get the path of cgi-bin executable
	std::string executable_path = this->config.cgi[resource_path.substr(resource_path.find_last_of("."))].c_str();
	
	// we make a new vector to hold the environment variables
	std::vector<std::string> new_env;
	
	// we make new so that we can add the new environment variables and dont affect the original environment variables
	for (size_t i = 0; i < env.size(); i++)
		new_env.push_back(env[i]);

	// first we need to make environment variables REQUEST_METHOD and PATH_INFO to pass to the CGI script
	new_env.push_back("REQUEST_METHOD=DELETE");
	new_env.push_back("PATH_INFO=" + resource_path);
	
	// we need to add the cookie to the environment variables if it exists
	if(cookie.empty() == false)
	{
		std::stringstream ss(cookie);
		std::string item;
		while (std::getline(ss, item, ';'))
		{
			item = item[0] == ' ' ? item.substr(1) : item;
			new_env.push_back("HTTP_COOKIE_" + item);
		}
	}

	// we need to make a new char **argv
	// we need to add the executable path and the resource path
	char *argv[] = {my_strdup(executable_path), 
					my_strdup(resource_path),
					NULL};

	// we need to make a new char **envp
	char *envp[new_env.size() + 1];
	for (size_t i = 0; i < new_env.size(); i++)
		envp[i] = my_strdup(new_env[i]);
	envp[new_env.size()] = NULL;

	execute_CGI(executable_path.c_str(), argv, envp);

	for (size_t i = 0; i < new_env.size(); i++)
		delete envp[i];

	for (size_t i = 0; i < 2; i++)
		delete argv[i];

}

void Client::delete_file(std::string &resource_path)
{
	if (std::remove(resource_path.c_str()) != 0)
	{
		// if the file is not deleted we send a 500 response
		send_error_response(500);
		return;
	}

	// if the file is deleted we send a 204 response
	// because the file is deleted and there is no content to send
	this->set_status_code(204)
		.set_body("")
		.build_response();
}

int Client::recursive_deletion(std::string path)
{
	// we define the directory pointer and the directory entry
	DIR *dir;
	struct dirent *entry;

	// we open the directory
	dir = opendir(path.c_str());

	// we check if the directory is opened
	if (dir == NULL)
		return -1;

	// we loop through the directory entries
	while ((entry = readdir(dir)))
	{
		// we skip the current directory and the parent directory
		std::string name = entry->d_name;
		if (name == "." || name == "..")
			continue;

		// we get the full path of the entry
		std::string full_path = path + "/" + entry->d_name;

		// we check if the entry is a directory
		if (entry->d_type == DT_DIR)
		{
			// we recursively delete the directory
			if (recursive_deletion(full_path) != 0)
				return -1;
		}
		// if the entry is a file we delete the file
		else
		{
			if(std::remove(full_path.c_str()) != 0)
				return -1;
		}
	}

	closedir(dir);

	// we delete the directory
	if (std::remove(path.c_str()) != 0)
		return -1;

	return 0;
}