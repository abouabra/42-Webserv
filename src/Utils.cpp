#include "../includes/Utils.hpp"
#include <iostream>

void log(std::string message, int color)
{
	time_t current;
	char buffer[100];

	current = std::time(NULL);
	std::memset(buffer, 0, sizeof(buffer));
	strftime(buffer, 100, "[%Y-%m-%d %H:%M:%S]", localtime(&current));


	std::map<int, std::string> log_colors;

	log_colors[RED] = "\033[1;31m";	// red
	log_colors[GREEN] = "\033[1;32m"; // green
	log_colors[YELLOW] = "\033[1;33m"; // yellow
	log_colors[BLUE] = "\033[1;34m";	// blue
	log_colors[MAGENTA] = "\033[1;35m"; // magenta
	log_colors[CYAN] = "\033[1;36m"; // cyan
	log_colors[WHITE] = "\033[1;37m"; // white
	log_colors[RESET] = "\033[0m"; // reset

	std::cout << log_colors[color] <<  buffer << " " << message << log_colors[RESET] <<std::endl;
}


std::string itoa(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}


std::string read_file(std::string name)
{
	std::fstream file(name.c_str(), std::ios::in);
	std::string content;
	std::string line;


	while(std::getline(file, line))
	{
		content += line;
		if(file.peek() != EOF)
			content += "\n";
	}

	file.close();
	return content;
}



int count_c(std::string str, char c)
{
	int count = 0;
	for(size_t i = 0; i < str.length(); i++)
	{
		if(str[i] == c)
			count++;
	}
	return count;
}


int ft_atoi(std::string str)
{
	int num = 0;
	int sign = 1;
	if(str[0] == '-')
	{
		sign = -1;
		str.erase(0, 1);
	}
	for(size_t i = 0; i < str.length(); i++)
	{
		if(!isdigit(str[i]))
			return -1;
		num = num * 10 + (str[i] - '0');
	}
	return num * sign;
}


int ip_to_int(std::string &host)
{
	std::stringstream ss(host);
	std::string token;
	int ip = 0;
	
	for (int i = 0; i < 4; i++)
	{
		std::getline(ss, token, '.');
		ip = ip << 8;
		ip += ft_atoi(token);
	}
	return ip;
}


std::string int_to_ip(int host)
{
	std::string ip;
	for (int i = 0; i < 4; i++)
	{
		ip = itoa(host & 0xFF) + ip;
		if (i != 3)
			ip = "." + ip;
		host = host >> 8;
	}
	return ip;
}



void print_ascii_header()
{
	// WEBSERV in ASCII art
	std::cout << "\033[1;33m";
	std::cout << "####################################################################################" << std::endl;
	std::cout << "#                                                                                  #" << std::endl;
	std::cout << "#     /$$      /$$           /$$                                                   #" << std::endl;
	std::cout << "#    | $$  /$ | $$          | $$                                                   #" << std::endl;
	std::cout << "#    | $$ /$$$| $$  /$$$$$$ | $$$$$$$   /$$$$$$$  /$$$$$$   /$$$$$$  /$$    /$$    #" << std::endl;
	std::cout << "#    | $$/$$ $$ $$ /$$__  $$| $$__  $$ /$$_____/ /$$__  $$ /$$__  $$|  $$  /$$/    #" << std::endl;
	std::cout << "#    | $$$$_  $$$$| $$$$$$$$| $$  \\ $$|  $$$$$$ | $$$$$$$$| $$  \\__/ \\  $$/$$/     #" << std::endl;
	std::cout << "#    | $$$/ \\  $$$| $$_____/| $$  | $$ \\____  $$| $$_____/| $$        \\  $$$/      #" << std::endl;
	std::cout << "#    | $$/   \\  $$|  $$$$$$$| $$$$$$$/ /$$$$$$$/|  $$$$$$$| $$         \\  $/       #" << std::endl;
	std::cout << "#    |__/     \\__/ \\_______/|_______/ |_______/  \\_______/|__/          \\_/        #" << std::endl;
	std::cout << "#                                                                                  #" << std::endl;
	std::cout << "#                           ##  by abouabra and mbaanni  ##                        #" << std::endl;
	std::cout << "#                                                                                  #" << std::endl;
	std::cout << "####################################################################################" << std::endl;
	std::cout << "\033[0m";
	std::cout << std::endl;																			
}