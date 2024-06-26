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

	// loop and add extra servers for each port
	std::vector<ServerConfig> extra_servers;
	for (size_t i = 0; i < servers.size(); i++)
	{
		for (size_t j = 1; j < servers[i].port.size(); j++)
		{
			ServerConfig new_server(servers[i]);
			new_server.port[0] = servers[i].port[j];
			extra_servers.push_back(new_server);
		}
	}
	for (size_t i = 0; i < extra_servers.size(); i++)
		servers.push_back(extra_servers[i]);


	for (size_t i = 0; i < servers.size(); i++)
	{
		if (servers[i].host == -1 && servers[i].port.empty() == true)
		{
			std::cout << "Server #" << i << std::endl;
			throw(std::runtime_error("Missing host or port"));
		}

		if (servers[i].host == -1)
			servers[i].host = resolve_host(DEFAULT_HOST);

		if (servers[i].port.empty() == true)
			servers[i].port.push_back(DEFAULT_PORT);

		if (servers[i].root.empty() == true)
			servers[i].root = DEFAULT_ROOT;

		if (servers[i].index.empty())
			servers[i].index = DEFAULT_INDEX_FILE;

		if ((int)servers[i].max_body_size == -1)
			servers[i].max_body_size = DEFAULT_MAX_BODY_SIZE;
	}

	for(size_t i = 0; i < servers.size(); i++)
	{
		for (size_t j = i + 1; j  < servers.size(); j++)
		{
			if (servers[i].host == servers[j].host && servers[i].port[0] == servers[j].port[0])
			{
				for (size_t k = 0; k < servers[i].server_names.size(); k++)
				{
					if(std::find(servers[j].server_names.begin(), servers[j].server_names.end(), servers[i].server_names[k]) != servers[j].server_names.end())
					{
						std::cout << "Server #" << j << std::endl;
						throw(std::runtime_error("duplicated server_name"));
					}
				}
			}
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
		if (ss_2.peek() != EOF && (key != "port:" && key != "server_name:"))
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

			new_server.host = resolve_host(value);
			if (new_server.host == -1)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("Cold not resolve hostname"));
			}
		}

		else if (key == "port:")
		{
			std::string result;
			assign_if_valid(key, value, result, is_port_valid);
			new_server.port.push_back(ft_atoi(result));
			while(ss_2.peek() != EOF && ss_2 >> value)
			{
				assign_if_valid(key, value, result, is_port_valid);
				new_server.port.push_back(ft_atoi(result));
			}
			std::sort(new_server.port.begin(), new_server.port.end());
			if (std::adjacent_find(new_server.port.begin(), new_server.port.end()) != new_server.port.end())
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated port"));
			}
		}

		else if (key == "server_name:")
		{
			new_server.server_names.push_back(value);
			while(ss_2.peek() != EOF && ss_2 >> value)
			{
				if (std::find(new_server.server_names.begin(), new_server.server_names.end(), value) != new_server.server_names.end())
				{
					std::cout << line << std::endl;
					throw(std::runtime_error("duplicated server_name"));
				}
				new_server.server_names.push_back(value);
			}
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
			if ((int)new_server.max_body_size != -1)
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
			if(location.methods.empty())
				location.methods.push_back("GET");

			new_server.locations.push_back(location);
		}
		else {
			std::cout << line << std::endl;
			throw std::runtime_error("Invalid config syntax");
		}
	}

	for(size_t i = 0; i< new_server.locations.size(); i++)
	{
		for(size_t j = i + 1; j < new_server.locations.size(); j++)
		{
			if(new_server.locations[i].path == new_server.locations[j].path)
			{
				std::cout << "Location #" << j << std::endl;
				throw(std::runtime_error("duplicated location path"));
			}

		}
		if(new_server.locations[i].root.empty() == false && new_server.locations[i].alias.empty() == false)
		{
			std::cout << "Location #" << i << std::endl;
			throw(std::runtime_error("conflicting root and alias"));
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

// bool is_host_valid(std::string &host)
// {
// 	//if the host is empty or doesn't contain 3 dots we return false
// 	if (host.empty())
// 		return false;
// 	/*if(count_c(host, '.') != 3)
// 		return false;

// 	// here we check if the host contains only digits and dots
// 	if(host.find_first_not_of("0123456789.") != std::string::npos)
// 		return false;

// 	// here we split the host into tokens
// 	std::stringstream ss(host);
// 	std::string token;
	
// 	// here we loop through the the 4 tokens
// 	for (int i = 0; i < 4; i++)
// 	{
// 		std::getline(ss, token, '.');
// 		if (token.empty())
// 			return false;
// 		if (ft_atoi(token) < 0 || ft_atoi(token) > 255)
// 			return false;
// 	}

// 	if (ss.peek() != EOF)
// 		return false;*/
// 	return true;
// }

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
	long size = ft_atol(max_body_size);
	if (size < 0 || size > 500000000)
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
			if(location.root.empty() == false)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated root"));
			}
			assign_if_valid(key, value, location.root, is_root_valid);
		}

		else if (key == "alias:")
		{
			if(location.alias.empty() == false)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated alias"));
			}
			assign_if_valid(key, value, location.alias, is_root_valid);
		}

		else if (key == "index:")
		{
			if(location.index.empty() == false)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated index"));
			}
			assign_if_valid(key, value, location.index, is_index_valid);
		}

		else if (key == "methods:")
		{
			if(location.methods.empty() == false)
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated methods"));
			}

			if (is_valid_method(value))
				location.methods.push_back(value);
			else
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config method"));
			}

			while(ss_2.peek() != EOF && ss_2 >> value)
			{
				if (is_valid_method(value) == false)
				{
					std::cout << line << std::endl;
					throw(std::runtime_error("Invalid config method"));
				}
				location.methods.push_back(value);
			}


			// then we parse the rest of the methods
			// parse_location_methods(ss_2, line, location.methods);

			// // check if the total number of pipes matches the number of methods on the vector -1
			// if(count_c(line, '|') != (int) location.methods.size() - 1)
			// {
			// 	std::cout << line << std::endl;
			// 	throw(std::runtime_error("Invalid config method"));
			// }

			// check if there is duplicate methods
			// first we sort the vector
			// then we check if there are any adjacent duplicate methods
			std::sort(location.methods.begin(), location.methods.end());
			if (std::adjacent_find(location.methods.begin(), location.methods.end()) != location.methods.end())
			{
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config method"));
			}

			// // Check if the last value read was a separator
			// if (!ss_2.eof())
			// {
			// 	std::cout << line << std::endl;
			// 	throw(std::runtime_error("Invalid config method separator"));
			// }
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
		else
		{
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid config syntax"));
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

// void parse_location_methods(std::stringstream &ss_2, std::string &line, std::vector<std::string> &methods)
// {
// 	std::string value;

// 	while (ss_2 >> value)
// 	{
// 		// Check pipe order
// 		if (value == "|")
// 		{
// 			passed_pipes++;

// 			// Check if there is a method before the pipe
// 			if (methods.size() != passed_pipes)
// 			{
// 				std::cout << line << std::endl;
// 				throw(std::runtime_error("Invalid config method"));
// 			}
// 			continue;
// 		}

// 		else if (!is_valid_method(value))
// 		{
// 			std::cout << line << std::endl;
// 			throw(std::runtime_error("Invalid config method"));
// 		}
// 		methods.push_back(value);
// 	}
// }


/*

	SERVER CONFIG CLASS

*/
ServerConfig::ServerConfig()
{
	// setting up default values
	host = -1;
	max_body_size = -1;
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
		server_names = obj.server_names;
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
		alias = obj.alias;
		methods = obj.methods;
		redirect_URL = obj.redirect_URL;
		directory_listing = obj.directory_listing;
		upload_dir = obj.upload_dir;
	}
	return *this;
}