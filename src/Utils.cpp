#include "../includes/Utils.hpp"

void log(std::string message, int level)
{
	time_t current;
	char buffer[100];

	current = std::time(NULL);
	std::memset(buffer, 0, sizeof(buffer));
	strftime(buffer, 100, "[%Y-%m-%d %H:%M:%S]", localtime(&current));


	std::map<int, std::string> log_colors;

	log_colors[INFO] = "\033[1;32m";
	log_colors[WARNING] = "\033[1;33m";
	log_colors[ERROR] = "\033[1;31m";
	log_colors[RESET] = "\033[0m";

	std::cout << log_colors[level] <<  buffer << " " << message << log_colors[RESET] <<std::endl;
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
