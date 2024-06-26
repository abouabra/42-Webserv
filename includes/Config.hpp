#pragma once
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include "Utils.hpp"
#include "WebServ.hpp"

class Location {
public:
	std::string path;
	std::string root;
	std::string alias;
	std::string index;
	std::vector<std::string> methods;
	std::string redirect_URL;
	bool directory_listing;
	std::string upload_dir;

	Location();
	~Location();
	Location(Location const &src);	
	Location &operator=(Location const &obj);
};

class ServerConfig {
public:
	int host;
	std::vector<int> port;
	std::vector<std::string> server_names;
	std::string root;
	size_t max_body_size;
	std::string index;
	std::map<int, std::string> error_pages;
	std::map<std::string, std::string> cgi;
	std::vector<Location> locations;

	ServerConfig();
	~ServerConfig();
	ServerConfig(ServerConfig const &src);
	ServerConfig &operator=(ServerConfig const &obj);

};

class Config {
private:
	std::string config_file;

public:
	~Config();
	Config();
	Config(std::string file_name);
	Config(Config const &src);
	Config &operator=(Config const &obj);

	std::vector<ServerConfig> servers;

	std::string read_config(std::string &file_name);
	void clean_file(std::string& config);
	void parse_config(std::string &config_file);
	void print_config();

	
};

ServerConfig parse_server_config(std::stringstream &ss);
void assign_if_valid(std::string &key, std::string &value, std::string &assign_to, bool (*is_valid)(std::string &));

bool is_host_valid(std::string &host);
bool is_port_valid(std::string &port);
bool is_root_valid(std::string &root);
bool is_max_body_size_valid(std::string &max_body_size);
bool is_index_valid(std::string &index);
std::map<int, std::string> parse_error_pages(std::stringstream &ss);
std::map<std::string, std::string> parse_cgi(std::stringstream &ss);


Location parse_server_location(std::stringstream &ss);
bool is_location_path_valid(std::string &path);
bool is_redirect_URL_valid(std::string &path);
void parse_location_methods(std::stringstream &ss_2, std::string &line, std::vector<std::string> &methods);
bool is_valid_method(std::string &method);

