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
	
	// here we will read the config file and store it in a string
	config_file = read_config(file_name);
	
	// here we clean the file by removing comments and empty lines
	clean_file(config_file);
	
	// here we parse and checks the config file and store the values in the Config object
	parse_config(config_file);

	// log("Config file: " + file_name, INFO);

	// here we print the config file
	print_config();
}

std::string Config::read_config(std::string &config_file)
{
	// here we open the config file
	std::fstream file(config_file.c_str(), std::ios::in);
	
	// here we check if the file is open
	// if not we throw an exception
	if(!file.is_open())
	{
		// log("Error: Could not open config file", ERROR);
		throw std::runtime_error("Could not open config file");
	}

	// here we read the content of the file and store it in a string
	std::string content;
	std::string line;

	while(std::getline(file, line))
	{
		// here we add the line to the content
		content += line;

		// here we check if we reached the end of the file
		// if not we add a newline character
		if (file.peek() != EOF)
			content += "\n";
	}

	// here we close the file and return the content
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
	// here we start by turning the string into a stringstream
	// then we loop through the lines of the config file
	std::stringstream ss(config_file);
	std::string line;

	while(std::getline(ss, line))
	{
		// here we check for the server keyword
		// it has to be the first keyword for a server block
		if(line != "server:")
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid server block"));
		}
		else if (line == "server:")
		{
			// here we call the parse_server_config function
			// it parses the entire server block and returns a ServerConfig object
			// it will also move the stringstream to the next server block
			ServerConfig server = parse_server_config(ss);
			servers.push_back(server);
		}
	}
}


ServerConfig parse_server_config(std::stringstream &ss)
{
	ServerConfig new_serve;
	std::string line;
	std::string key;
	std::string value;

	// here we loop through the lines of the server block
	while (std::getline(ss, line))
	{
		// here we check if we reached the start of a new server block
		// then we move the stringstream back to the start of the line
		// and break the loop
		if(line == "server:")
		{
			int i = ss.tellg();
			ss.seekg(i - line.size() - 1, std::ios::beg);
			break;
		}

		// here we check for the number of tabs
		// inside a server block, there should be only one tab
		// otherwise we throw an exception
		int tabs = count_c(line, '\t');
		if (tabs != 1)
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid server config tabs"));
		}

		// here we parse the line
		// we split the line into a key and a value
		std::stringstream ss_2(line);
		ss_2 >> key;
		ss_2 >> value;

		// here we check if there is still data on the line
		// if there is we throw an exception
		if (ss_2.peek() != EOF)
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid syntax"));
		}

		// we check each key to see if its valid
		// if not we throw an exception otherwise we store the keyword value
		
		
		// here we parse the host keyword
		if (key == "host:")
		{
			// check duplicate
			if (new_serve.host != -1)
			{
				print_error_at_line_x(ss);
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated host"));
			}

			// here we check if the host is valid
			std::string result;
			assign_if_valid(key, value, result, is_host_valid, ss);
			new_serve.host = ip_to_int(result);
		}

		// here we parse the port keyword
		else if (key == "port:")
		{
			// check duplicate
			if (new_serve.port != 0)
			{
				print_error_at_line_x(ss);
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated port"));
			}

			// here we check if the port is valid
			std::string result;
			assign_if_valid(key, value, result, is_port_valid, ss);
			new_serve.port = ft_atoi(result);
		}

		// here we parse the root keyword
		else if (key == "root:")
		{
			// check duplicate
			if (new_serve.root.empty() == false)
			{
				print_error_at_line_x(ss);
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated root"));
			}

			// here we check if the root is valid
			assign_if_valid(key, value, new_serve.root, is_root_valid, ss);
		}

		// here we parse the max body size keyword
		else if (key == "max_body_size:")
		{
			// check duplicate
			if (new_serve.max_body_size != 1000000)
			{
				print_error_at_line_x(ss);
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated max_body_size"));
			}

			// here we check if the max body size is valid
			std::string result;
			assign_if_valid(key, value, result, is_max_body_size_valid, ss);
			new_serve.max_body_size = ft_atoi(result);
		}

		// here we parse the index keyword on the server block
		else if (key == "index:")
		{
			// check duplicate
			if (new_serve.index.empty() == false)
			{
				print_error_at_line_x(ss);
				std::cout << line << std::endl;
				throw(std::runtime_error("duplicated index"));
			}

			// here we check if the index is valid
			assign_if_valid(key, value, new_serve.index, is_index_valid, ss);
		}

		// here we parse the error pages keyword and its sub keys
		else if (line == "\terror_pages:")
		{
			new_serve.error_pages = parse_error_pages(ss);
		}

		// here we parse the cgi keyword and its sub keys
		else if (key == "cgi:")
		{
			new_serve.cgi = parse_cgi(ss);
		}

		// here we parse the location keyword and its sub keys
		else if (key == "location:")
		{
			Location location;
			// here we check if the location path is valid
			if (is_location_path_valid(value) == false)
			{
				print_error_at_line_x(ss);
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid location path"));
			}

			// then we parse the location block
			location = parse_server_location(ss);
			location.path = value;
			new_serve.locations.push_back(location);
		}
		else {
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw std::runtime_error("Invalid config syntax");
		}
	}
	return new_serve;
}

void assign_if_valid(std::string &key, std::string &value, std::string &assign_to, bool (*is_valid)(std::string &), std::stringstream &ss)
{
	// this function checks if the value is valid
	// using the is_valid function pointer
	// if the value is valid we assign it to the assign_to variable
	// otherwise we throw an exception

	if (is_valid(value) == true)
		assign_to = value;
	else
	{
		print_error_at_line_x(ss);
		std::cout << key << " " << value << std::endl;
		throw(std::runtime_error("Invalid " + key));
	}
}

void print_error_at_line_x(std::stringstream &ss)
{
	//here we print the line number where the error occured

	// we store the current position of the stringstream
	std::streampos pos = ss.tellg();

	// we move the stringstream to the start
	ss.seekg(0, std::ios::beg);

	// we loop through the lines of the stringstream
	// and count the lines
	int line_number = 0;
	std::string line;
	while (std::getline(ss, line))
	{
		line_number++;

		// if we reached the position we print the line number
		if (ss.tellg() == pos)
		{
			std::cout << "Error During Parsing at Line: " << line_number << std::endl;
			break;
		}
	}
}

bool is_host_valid(std::string &host)
{
	// here we check if the host is valid
	
	//if the host is empty or doesn't contain 3 dots we return false
	if (host.empty())
		return false;
	if(count_c(host, '.') != 3)
		return false;

	// here we split the host into tokens
	std::stringstream ss(host);
	std::string token;
	
	// here we loop through the the 4 tokens
	for (int i = 0; i < 4; i++)
	{
		std::getline(ss, token, '.');
		
		// here we check if the token is empty
		if (token.empty())
			return false;
		
		// here we check if the token contains only digits
		if(host.find_first_not_of("0123456789.") != std::string::npos)
			return false;
		
		// here we check if the token is a valid number
		if (ft_atoi(token) < 0 || ft_atoi(token) > 255)
			return false;
	}
	// here we check if there are any characters left in the stringstream
	if (ss.peek() != EOF)
		return false;
	return true;
}

bool is_port_valid(std::string &port)
{
	// here we check if the port is valid
	// if the port is empty or contains any non digit characters we return false
	if (port.empty())
		return false;
	if (port.find_first_not_of("0123456789") != std::string::npos)
		return false;
	
	// here we check if the port is a valid number
	if (ft_atoi(port) < 0 || ft_atoi(port) > 65535)
		return false;
	return true;
}

bool is_root_valid(std::string &root)
{
	// here we check if the root is valid
	// if the root is empty we return false
	if (root.empty())
		return false;

	// here we check if the root is a valid path
	// if the path doesn't start with a / we return false
	// also if the path doesn't end with a / we return false
	if (root[0] != '/' || root[root.size() - 1] != '/')
		return false;

	// here we check if the path contains any invalid characters
	// if it does we return false
	if (root.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
		return false;

	return true;
}

bool is_max_body_size_valid(std::string &max_body_size)
{
	// here we check if the max body size is valid
	// if the max body size is empty or contains any non digit characters we return false
	if (max_body_size.empty())
		return false;
	if (max_body_size.find_first_not_of("0123456789") != std::string::npos)
		return false;
	
	// here we check if the max body size is a valid number
	if (ft_atoi(max_body_size) < 0)
		return false;
	return true;
}

bool is_index_valid(std::string &index)
{
	// here we check if the index is valid
	// if the index is empty we return false
	if (index.empty())
		return false;

	// here we check if the path contains any invalid characters
	// if it does we return false
	if (index.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
		return false;

	return true;
}


std::map<int, std::string> parse_error_pages(std::stringstream &ss)
{
	// here we parse the error pages
	std::map<int, std::string> error_pages;
	std::string line;

	// here we loop through the lines of the error pages block
	while (std::getline(ss, line))
	{
		std::stringstream ss_2(line);
		std::string key;
		std::string value;
		ss_2 >> key;
		ss_2 >> value;

		// here we check if we reached the end of the error pages block
		// if we did we move the stringstream back to the start of the line
		if (line == "server:" || key == "cgi:" || key == "location:")
		{
			int i = ss.tellg();
			ss.seekg(i - line.size() - 1, std::ios::beg);
			break;
		}

		// here we check if the key is a valid number
		if (key.empty())
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page code"));
		}

		// if key ends with a colon, we remove it
		if (key[key.size() - 1] == ':')
			key = key.substr(0, key.size() - 1);
		else
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page code"));
		}

		// here we check if the key is a valid key
		if (key.find_first_not_of("0123456789") != std::string::npos)
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page code"));
		}

		int code = ft_atoi(key);
		// here we check if the code is a valid code
		if (code < 100 || code > 599)
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page code"));
		}

		// here we check if there is a value
		if (value.empty())
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page path"));
		}

		// here we check if the value is a valid path
		if (value.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page path"));
		}

		// we check if there still data on the line
		if (ss_2.peek() != EOF)
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid config syntax"));
		}

		// check if there is duplicate error codes
		// if there are we throw an exception
		if (error_pages.find(code) != error_pages.end())
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("duplicated error page code"));
		}

		// here we store the error page in the map
		error_pages[code] = value;
	}
	return error_pages;
}


std::map<std::string, std::string> parse_cgi(std::stringstream &ss)
{
	// here we parse the cgi block
	std::map<std::string, std::string> cgi;
	std::string line;
	std::string key;
	std::string value;

	// here we loop through the lines of the cgi block
	while (std::getline(ss, line))
	{
		std::stringstream ss_2(line);
		ss_2 >> key;
		ss_2 >> value;

		// here we check if we reached the end of the cgi block
		if (line == "server:" || key == "error_pages:" || key == "location:")
		{
			int i = ss.tellg();
			ss.seekg(i - line.size() - 1, std::ios::beg);
			break;
		}

		// here we check if the key is empty
		if (key.empty())
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi key"));
		}

		// here we check if the key (extension) is a valid key
		if (key.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.:") != std::string::npos)
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi key"));
		}

		// if key ends with a colon, we remove it
		if (key[key.size() - 1] == ':')
			key = key.substr(0, key.size() - 1);
		else
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid error page code"));
		}

		// here we check if the key is starting with a dot
		if (key[0] != '.')
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi key"));
		}

		// here we check if the value is empty
		if (value.empty())
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi value"));
		}

		// here we check if the value is a valid path
		if (value.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi value"));
		}

		// here we check if the path starts with a /
		if (value[0] != '/')
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid cgi value"));
		}

		// we check if there still data on the line
		if (ss_2.peek() != EOF)
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid config syntax"));
		}

		//check if there is duplicate cgi keys
		// if there are we throw an exception
		if (cgi.find(key) != cgi.end())
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("duplicated cgi key"));
		}

		// here we store the cgi in the map
		cgi[key] = value;
	}
	return cgi;
}

bool is_location_path_valid(std::string &path)
{
	// here we check if the path is valid
	// if the path is empty we return false
	if (path.empty())
		return false;

	// here we check if the path is a valid path
	// if the path doesn't start with a / we return false
	if (path[0] != '/')
		return false;

	// here we check if the path contains any invalid characters
	// if it does we return false
	if (path.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_/.") != std::string::npos)
		return false;

	return true;
}

void Config::print_config()
{
	// here we loop through the servers and print their values

	for(size_t i = 0; i < servers.size(); i++)
	{
		std::cout << "Server #" << i << std::endl;
		std::cout << "\tHost: " << int_to_ip(servers[i].host) << std::endl;
		std::cout << "\tPort: " << servers[i].port << std::endl;
		std::cout << "\tRoot: " << servers[i].root << std::endl;
		std::cout << "\tMax body size: " << servers[i].max_body_size << std::endl;
		std::cout << "\tIndex: " << servers[i].index << std::endl;
		std::cout << "\tError pages: " << std::endl;

		for (std::map<int, std::string>::iterator it = servers[i].error_pages.begin(); it != servers[i].error_pages.end(); it++)
			std::cout << "\t\t" << it->first << " => " << it->second << std::endl;
		std::cout << "\tCGI: " << std::endl;
		
		for (std::map<std::string, std::string>::iterator it = servers[i].cgi.begin(); it != servers[i].cgi.end(); it++)
			std::cout << "\t\t" << it->first << " => " << it->second << std::endl;
		
		for (size_t j = 0; j < servers[i].locations.size(); j++)
		{
			std::cout << "\tLocation #" << j << std::endl;
			std::cout << "\t\tPath: " << servers[i].locations[j].path << std::endl;
			std::cout << "\t\tRoot: " << servers[i].locations[j].root << std::endl;
			std::cout << "\t\tIndex: " << servers[i].locations[j].index << std::endl;
			std::cout << "\t\tMethods: ";
			
			for (size_t k = 0; k < servers[i].locations[j].methods.size(); k++)
				std::cout << servers[i].locations[j].methods[k] << " ";
			std::cout << std::endl;
			
			std::cout << "\t\tRedirect url: " << servers[i].locations[j].redirect_url << std::endl;
			std::cout << "\t\tDirectory listing: " << servers[i].locations[j].directory_listing << std::endl;
			std::cout << "\t\tUpload enabled: " << servers[i].locations[j].upload_enabled << std::endl;
			std::cout << "\t\tUpload directory: " << servers[i].locations[j].upload_directory << std::endl;
			std::cout << "\t\tUpload max size: " << servers[i].locations[j].upload_max_size << std::endl;
		}
	}
}



Location parse_server_location(std::stringstream &ss)
{
	// here we parse the location block
	Location location;
	std::string line;

	// here we loop through the lines of the location block
	while (std::getline(ss, line))
	{
		std::stringstream ss_2(line);
		std::string key;
		std::string value;

		// here we extract the key and value from the line
		ss_2 >> key;
		ss_2 >> value;

		// here we check if we reached the end of the location block
		// if we did we move the stringstream back to the start of the line
		if (key == "server:" || key == "location:" || key == "cgi:" || key == "error_pages:")
		{
			int i = ss.tellg();
			ss.seekg(i - line.size() - 1);
			break;
		}

		// here we check if there is still data on the line
		// we have to check if the key is not methods because it can have multiple values
		if (ss_2.peek() != EOF && key != "methods:")
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid config syntax"));
		}

		// here we check the location root keyword
		// it has the same check as the server root
		if (key == "root:")
		{
			assign_if_valid(key, value, location.root, is_root_valid, ss);
		}

		// here we check the location index keyword
		// it has the same check as the server index
		else if (key == "index:")
		{
			assign_if_valid(key, value, location.index, is_index_valid, ss);
		}

		//here we check the location methods keyword
		else if (key == "methods:")
		{
			// we check the initial value if it is a valid method
			if (is_valid_method(value))
				location.methods.push_back(value);
			else
			{
				print_error_at_line_x(ss);
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config method"));
			}

			// then we parse the rest of the methods
			parse_location_methods(ss, ss_2, line, location.methods);

			// check if the total number of pipes matches the number of methods on the vector -1
			size_t total_pipe_count = count_c(line, '|');
			if(total_pipe_count != location.methods.size() - 1)
			{
				print_error_at_line_x(ss);
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config method"));
			}

			//check if there is duplicate methods
			// first we sort the vector
			// then we check if there are any adjacent duplicate methods
			// if there are we throw an exception
			std::sort(location.methods.begin(), location.methods.end());
			if (std::adjacent_find(location.methods.begin(), location.methods.end()) != location.methods.end())
			{
				print_error_at_line_x(ss);
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config method"));
			}

			// Check if the last value read was a separator
			if (!ss_2.eof() && value != "|")
			{
				print_error_at_line_x(ss);
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config method separator"));
			}
		}
		else if (key == "redirect_url:")
		{
			location.redirect_url = value;
			// check_multiple(value, ss_2);
		}
		else if (key == "directory_listing:")
		{
			if (value == "false")
				location.directory_listing = false;
			else if (value == "true")
				location.directory_listing = true;
			else
			{
				print_error_at_line_x(ss);
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config directory_listing"));
			}
		}
		else if (key == "upload_enabled:")
		{
			if (value == "false")
				location.upload_enabled = false;
			else if (value == "true")
				location.upload_enabled = true;
			else
			{
				print_error_at_line_x(ss);
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config upload_enabled"));
			}
		}
		else if (key == "upload_directory:")
		{
			location.upload_directory = value;
		}
		else if (key == "upload_max_size:")
		{
			location.upload_max_size = ft_atoi(value);
		}
	}
	return location;
}


bool is_valid_method(std::string &method)
{
	// here we check if the method is valid
	// if the method is empty we return false
	if (method.empty())
		return false;

	// here we check if the method is a valid method
	// if the method is not GET, POST, DELETE, PUT, HEAD, OPTIONS we return false
	if (method != "GET" && method != "POST" && method != "DELETE" && method != "PUT" && method != "HEAD" && method != "OPTIONS" && method != "TRACE")
		return false;
	return true;
}



void parse_location_methods(std::stringstream &ss, std::stringstream &ss_2, std::string &line, std::vector<std::string> &methods)
{
	size_t passed_pipes = 0;
	std::string value;

	// we loop and extract the methods from the stringstream
	while (ss_2 >> value)
	{
		// Check pipe order
		if (value == "|")
		{
			passed_pipes++;

			// Check if there is a method before the pipe
			if (methods.size() != passed_pipes)
			{
				print_error_at_line_x(ss);
				std::cout << line << std::endl;
				throw(std::runtime_error("Invalid config method"));
			}

			// Skip separator and continue parsing
			continue;
		}

		// Check if there is methods are valid
		else if (!is_valid_method(value))
		{
			print_error_at_line_x(ss);
			std::cout << line << std::endl;
			throw(std::runtime_error("Invalid config method"));
		}
		
		// Store valid method
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
	port = 0;
	max_body_size = 1000000;
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
	upload_enabled = false;
	upload_max_size = 1000000; // 1MB
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
		redirect_url = obj.redirect_url;
		directory_listing = obj.directory_listing;
		upload_enabled = obj.upload_enabled;
		upload_directory = obj.upload_directory;
		upload_max_size = obj.upload_max_size;
	}
	return *this;
}