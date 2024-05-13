#include "../includes/Config.hpp"
#include <cstddef>
#include <map>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

/*
	CONFIG CLASS
*/

Config::Config()
{

}

Config::~Config()
{

}

Config::Config(Config const &src)
{
	*this = src;
}

Config& Config::Config::operator=(Config const &obj)
{
	if (this != &obj)
	{
		servers = obj.servers;
		config_file = obj.config_file;
	}
	return *this;
}

Config::Config(std::string file_name)
{
	config_file = read_config(file_name);

	//removing comments and empty lines
	clean_file(config_file);

	// here we parse and checks the config file and store the values in the Config object
	parse_config(config_file);

	log("Config file: " + file_name, WHITE);
}

std::string Config::read_config(std::string &config_file)
{
	std::fstream file(config_file.c_str(), std::ios::in);

	if(!file.is_open())
	{
		throw std::runtime_error("Could not open config file");
	}

	std::string content;
	std::string line;

	while(std::getline(file, line))
	{
		content += line;

		// here we check if we reached the end of the file
		// if not we add a newline character
		if (file.peek() != EOF)
			content += "\n";
	}

	file.close();
	return content;
}


void Config::clean_file(std::string& config) {
	// here we remove comments #
	size_t pos = 0;
	while ((pos = config.find("#", pos)) != std::string::npos) {
		size_t end = config.find("\n", pos);
		config.erase(pos, end - pos);
	}

	// here we remove empty lines and trimming ending whitespaces
	std::istringstream iss(config);
	std::string line;
	config.clear();
	while (std::getline(iss, line)) {
		line.erase(line.find_last_not_of(" \t\n\v\f\r") + 1);

		if (line.find_first_not_of(" \t\n\v\f\r") != std::string::npos) {
			config += line;
			if (iss.peek() != EOF) {
				config += "\n";
			}
		}

	}
}

void Config::parse_config(std::string &config_file)
{
	std::stringstream ss(config_file);
	std::string line;

	while(std::getline(ss, line))
	{
		//we check for the server keyword
		//it has to be the first keyword for a server block
		if(line != "server:")
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid server block"));
		}
		else if (line == "server:")
		{
			// we parse the entire server block and returns a ServerConfig object
			// it will also move the stringstream to the next server block
			ServerConfig server = parse_server_config(ss);
			servers.push_back(server);
		}
	}

	// loop through the config and check for missing values
	for (size_t i = 0; i < servers.size(); i++)
	{
		// here we check if the server has all the required values
		if (servers[i].host == -1)
		{
			// here we check if the host is missing
			std::cout << "Server #" << i << std::endl;
			throw(std::runtime_error("Missing host"));
		}
		if (servers[i].port == -1)
		{
			// here we check if the port is missing
			std::cout << "Server #" << i << std::endl;
			throw(std::runtime_error("Missing port"));
		}
		if (servers[i].root.empty())
		{
			// here we check if the root is missing
			std::cout << "Server #" << i << std::endl;
			throw(std::runtime_error("Missing root"));
		}
		if (servers[i].index.empty())
		{
			// here we check if the index is missing
			std::cout << "Server #" << i << std::endl;
			throw(std::runtime_error("Missing index"));
		}
	}
}


ServerConfig parse_server_config(std::stringstream &ss)
{
	ServerConfig new_server;
	std::string line;
	std::string key;
	std::string value;

	while (std::getline(ss, line))
	{
		// here we check if we reached the start of a new server block
		// then we move the stringstream back to the start of the line
		// and break the loop
		if(line == "server:")
		{
			ss.seekg((size_t) ss.tellg() - line.size() - 1, std::ios::beg);
			break;
		}

		// here we check for the number of tabs
		if (count_c(line, '\t') != 1)
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid server config tabs"));
		}

		key.clear();
		value.clear();

		std::stringstream ss_2(line);
		ss_2 >> key;
		ss_2 >> value;


		// here we check if there is still data on the line
		if (ss_2.peek() != EOF)
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid syntax"));
		}

		// we check each key to see if its valid
		// if not we throw an exception otherwise we store the keyword value

		if (key == "host:")
		{
			// check duplicate
			if (new_server.host != -1)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated host"));
			}

			// here we check if the host is valid
			std::string result;
			assign_if_valid(key, value, result, is_host_valid);
			new_server.host = ip_to_int(result);
		}

		else if (key == "port:")
		{
			if (new_server.port != -1)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated port"));
			}

			// here we check if the port is valid
			std::string result;
			assign_if_valid(key, value, result, is_port_valid);
			new_server.port = ft_atoi(result);
		}

		else if (key == "root:")
		{
			if (new_server.root.empty() == false)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated root"));
			}
			assign_if_valid(key, value, new_server.root, is_root_valid);
		}

		else if (key == "max_body_size:")
		{
			if (new_server.max_body_size != DEFAULT_MAX_BODY_SIZE)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated max_body_size"));
			}

			std::string result;
			assign_if_valid(key, value, result, is_max_body_size_valid);
			new_server.max_body_size = ft_atoi(result);
		}

		else if (key == "index:")
		{
			if (new_server.index.empty() == false)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated index"));
			}

			assign_if_valid(key, value, new_server.index, is_index_valid);
		}

		else if (line == "\terror_pages:")
		{
			new_server.error_pages = parse_error_pages(ss);
		}

		else if (key == "cgi:")
		{
			new_server.cgi = parse_cgi(ss);
		}

		else if (key == "location:")
		{
			Location location;

			if (is_location_path_valid(value) == false)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid location path"));
			}

			location = parse_server_location(ss);

			location.path = value;
			// if the location methods are empty we add the GET method as default
			if(location.methods.empty())
				location.methods.push_back("GET");

			new_server.locations.push_back(location);
		}
		else {
			std::cout << line << std::endl;
			throw std::runtime_error("Invalid config syntax");
		}
	}
	return new_server;
}

void assign_if_valid(std::string &key, std::string &value, std::string &assign_to, bool (*is_valid)(std::string &))
{
	// this function checks if the value is valid
	// using the is_valid function pointer
	// if the value is valid we assign it to the assign_to variable

	if (is_valid(value) == true)
		assign_to = value;
	else
	{
		std::cout << key << " " << value << std::endl;
		throw(std::runtime_error("Invalid " + key));
	}
}

bool is_host_valid(std::string &host)
{
	//if the host is empty or doesn't contain 3 dots we return false
	if (host.empty())
		return false;
	if(count_c(host, '.') != 3)
		return false;

	// here we check if the host contains only digits and dots
	if(host.find_first_not_of("0123456789.") != std::string::npos)
		return false;

	// here we split the host into tokens
	std::stringstream ss(host);
	std::string token;
	
	// here we loop through the the 4 tokens
	for (int i = 0; i < 4; i++)
	{
		std::getline(ss, token, '.');
		if (token.empty())
			return false;
		if (ft_atoi(token) < 0 || ft_atoi(token) > 255)
			return false;
	}

	if (ss.peek() != EOF)
		return false;
	return true;
}

bool is_port_valid(std::string &port)
{
	if (port.empty())
		return false;
	if (port.find_first_not_of("0123456789") != std::string::npos)
		return false;
	
	// here we check if the port is a valid number
	if (ft_atoi(port) <= 0 || ft_atoi(port) > 65535)
		return false;

	return true;
}

bool is_root_valid(std::string &root)
{
	if (root.empty())
		return false;

	// if the path doesn't start with a /
	// also if the path doesn't end with a /
	if (root[0] != '/' || root[root.size() - 1] != '/')
		return false;

	if (root.find("//") != std::string::npos)
		return false;

	// here we check if the path contains any invalid characters
	if (root.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
		return false;

	return true;
}

bool is_max_body_size_valid(std::string &max_body_size)
{
	if (max_body_size.empty())
		return false;
	if (max_body_size.find_first_not_of("0123456789") != std::string::npos)
		return false;
	if (ft_atoi(max_body_size) < 0)
		return false;
	return true;
}

bool is_index_valid(std::string &index)
{
	if (index.empty())
		return false;

	if (index[0] == '/' || index[index.size() - 1] == '/')
		return false;

	if (index.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
		return false;

	return true;
}


std::map<int, std::string> parse_error_pages(std::stringstream &ss)
{
	std::map<int, std::string> error_pages;
	std::string line;

	while (std::getline(ss, line))
	{
		std::stringstream ss_2(line);
		std::string key;
		std::string value;
		ss_2 >> key;
		ss_2 >> value;

		if (count_c(line, '\t') != 2)
		{
			ss.seekg((size_t) ss.tellg() - line.size() - 1, std::ios::beg);
			break;
		}
		if (key.empty())
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page code"));
		}

		// if key ends with a colon and has only one colon
		// we remove it
		if (key[key.size() - 1] == ':' && count_c(key, ':') == 1)
			key = key.substr(0, key.size() - 1);
		else
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page code"));
		}

		if (key.find_first_not_of("0123456789") != std::string::npos)
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page code"));
		}

		int code = ft_atoi(key);
		if (code < 100 || code > 599)
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page code"));
		}

		if (value.empty())
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page path"));
		}

		if (value.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page path"));
		}

		if (value[0] == '/' || value[value.size() - 1] == '/')
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page path"));
		}
		
		if (value.find("//") != std::string::npos)
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page path"));
		}

		if (ss_2.peek() != EOF)
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid config syntax"));
		}

		// check if there is duplicate error codes
		if (error_pages.find(code) != error_pages.end())
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("duplicated error page code"));
		}

		error_pages[code] = value;
	}
	return error_pages;
}


std::map<std::string, std::string> parse_cgi(std::stringstream &ss)
{
	std::map<std::string, std::string> cgi;
	std::string line;
	std::string key;
	std::string value;

	while (std::getline(ss, line))
	{
		std::stringstream ss_2(line);
		ss_2 >> key;
		ss_2 >> value;

		if (count_c(line, '\t') != 2)
		{
			ss.seekg((size_t) ss.tellg() - line.size() - 1, std::ios::beg);
			break;
		}

		if (key.empty())
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi key"));
		}

		// here we check if the key (extension) is a valid key
		if (key.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.:") != std::string::npos)
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi key"));
		}

		if (key.find("//") != std::string::npos)
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi key"));
		}

		// if key ends with a colon, we remove it
		if (key[key.size() - 1] == ':' && count_c(key, ':') == 1)
			key = key.substr(0, key.size() - 1);
		else
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page code"));
		}

		// here we check if the key is starting with a dot
		if (key[0] != '.' && key.find('.') != 1)
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi key"));
		}

		if (value.empty())
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi value"));
		}

		if (value.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi value"));
		}

		if (value.find("//") != std::string::npos)
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi value"));
		}

		if (value[0] != '/' || value[value.size() - 1] == '/')
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi value"));
		}

		if (ss_2.peek() != EOF)
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid config syntax"));
		}

		if (cgi.find(key) != cgi.end())
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("duplicated cgi key"));
		}
		cgi[key] = value;
	}
	return cgi;
}

bool is_location_path_valid(std::string &path)
{
	if (path.empty())
		return false;

	if (path[0] != '/')
		return false;

	if(path[path.size() - 1] == '/' && path != "/")
		return false;

	if (path.find("//") != std::string::npos)
		return false;

	if (path.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
		return false;

	return true;
}

bool is_redirect_URL_valid(std::string &path)
{
	if (path.empty())
		return false;

	if (path.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.:") != std::string::npos)
		return false;

	return true;
}

Location parse_server_location(std::stringstream &ss)
{
	Location location;
	std::string line;

	while (std::getline(ss, line))
	{
		std::stringstream ss_2(line);
		std::string key;
		std::string value;

		ss_2 >> key;
		ss_2 >> value;

		if (count_c(line, '\t') != 2)
		{
			ss.seekg((size_t) ss.tellg() - line.size() - 1, std::ios::beg);
			break;
		}

		// we have to check if the key is not methods because it can have multiple values
		if (ss_2.peek() != EOF && key != "methods:")
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid config syntax"));
		}

		if (key == "root:")
		{
			assign_if_valid(key, value, location.root, is_root_valid);
		}

		else if (key == "index:")
		{
			assign_if_valid(key, value, location.index, is_index_valid);
		}

		else if (key == "methods:")
		{
			if (is_valid_method(value))
				location.methods.push_back(value);
			else
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config method"));
			}

			// then we parse the rest of the methods
			parse_location_methods(ss_2, line, location.methods);

			// check if the total number of pipes matches the number of methods on the vector -1
			if(count_c(line, '|') != (int) location.methods.size() - 1)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config method"));
			}

			// check if there is duplicate methods
			// first we sort the vector
			// then we check if there are any adjacent duplicate methods
			std::sort(location.methods.begin(), location.methods.end());
			if (std::adjacent_find(location.methods.begin(), location.methods.end()) != location.methods.end())
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config method"));
			}

			// Check if the last value read was a separator
			if (!ss_2.eof() && value != "|")
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config method separator"));
			}
		}
		else if (key == "redirect_URL:")
		{
			// check duplicate
			if (location.redirect_URL.empty() == false)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated redirect_URL"));
			}

			// here we check if the redirect url is a valid path
			assign_if_valid(key, value, location.redirect_URL, is_redirect_URL_valid);

		}
		else if (key == "directory_listing:")
		{
			// check duplicate
			if (location.directory_listing != false)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated directory_listing"));
			}

			if (value == "false")
				location.directory_listing = false;
			else if (value == "true")
				location.directory_listing = true;
			else
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config directory_listing"));
			}
		}
		else if (key == "upload_dir:")
		{
			// check duplicate
			if (location.upload_dir.empty() == false)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated upload_directory"));
			}

			assign_if_valid(key, value, location.upload_dir, is_root_valid);
		}
	}
	return location;
}


bool is_valid_method(std::string &method)
{
	if (method.empty())
		return false;
	if (method != "GET" && method != "POST" && method != "DELETE" && method != "PUT" && method != "HEAD" && method != "OPTIONS" && method != "TRACE")
		return false;
	return true;
}

void parse_location_methods(std::stringstream &ss_2, std::string &line, std::vector<std::string> &methods)
{
	size_t passed_pipes = 0;
	std::string value;

	while (ss_2 >> value)
	{
		// Check pipe order
		if (value == "|")
		{
			passed_pipes++;

			// Check if there is a method before the pipe
			if (methods.size() != passed_pipes)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config method"));
			}
			continue;
		}

		else if (!is_valid_method(value))
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid config method"));
		}
		methods.push_back(value);
	}
}


/*

	SERVER CONFIG CLASS

*/
ServerConfig::ServerConfig()
{
	// setting up default values
	host = -1;
	port = -1;
	max_body_size = DEFAULT_MAX_BODY_SIZE;
}

ServerConfig::~ServerConfig()
{

}

ServerConfig::ServerConfig(ServerConfig const &src)
{
	*this = src;
}

ServerConfig& ServerConfig::ServerConfig::operator=(ServerConfig const &obj)
{
	if (this != &obj)
	{
		host = obj.host;
		port = obj.port;
		root = obj.root;
		max_body_size = obj.max_body_size;
		index = obj.index;
		error_pages = obj.error_pages;
		cgi = obj.cgi;
		locations = obj.locations;
	}
	return *this;
}

/*

	Location CLASS

*/

Location::Location()
{
	// setting up default values
	directory_listing = false;
}

Location::~Location()
{

}

Location::Location(Location const &src)
{
	*this = src;
}

Location& Location::Location::operator=(Location const &obj)
{
	if (this != &obj)
	{
		path = obj.path;
		root = obj.root;
		index = obj.index;
		methods = obj.methods;
		redirect_URL = obj.redirect_URL;
		directory_listing = obj.directory_listing;
		upload_dir = obj.upload_dir;
	}
	return *this;
}