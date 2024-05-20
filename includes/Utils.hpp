#pragma once
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <cstring>
#include <netinet/in.h>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "WebServ.hpp"

enum log_colors
{
	RED,
	GREEN,
	BLUE,
	YELLOW,
	MAGENTA,
	CYAN,
	WHITE,
	RESET
};

//convert host name to ip address
int resolve_host(std::string host);

void log(std::string message, int color);

/*
	this function will convert an integer to a string
*/
std::string itoa(int i);

/*
	this function will read the content of a file and return it as a string
*/
std::string read_file(std::string name);

/*
	this function will count the number of times a character appears in a string
*/
int count_c(std::string str, char c);

/*
	this is an implementation of the atoi function
*/
int ft_atoi(std::string str);

/*
	this function will convert a ip from a string to an integer
*/
int ip_to_int(std::string &host);

/*
	this function will convert a ip from an integer to a string
*/
std::string int_to_ip(int host);

void print_ascii_header();

std::string convert_size(size_t size);

std::string extension_to_html_icon(std::string extension);

std::string GenerateUniqueFileName();

std::string decode_URL(std::string URL);

char* my_strdup(const std::string& str);

long ft_atol(std::string str);
