#pragma once
#include <cstddef>
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <cstring>
#include <netinet/in.h>
#include <ctime>

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