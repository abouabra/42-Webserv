#include "../includes/Config.hpp"
#include "../includes/Utils.hpp"
#include "../includes/Server.hpp"

/*
	this is the main function that will be called when the program is executed
	it will check if the user has provided a config file as an argument
	if not, it will use the default config file
	it will then read, parses, checks and then creates the Config object
	then it will create the Server object and run the server
*/


int main(int ac, char **av, char **ev)
{
	std::string file_name;

	// check if the user has provided more than 1 argument
	// if so, print the usage and return 1
	if(ac > 2)
	{
		std::cout << "Usage: ./webserv <config_file>" << std::endl;
		return 1;
	}

	// if the user has provided a config file, use it
	// otherwise use the default config file
	if(ac == 2)
		file_name = av[1];
	else
		file_name = "./config/new.conf";

	// try to start the server
	// if an exception is thrown, print the error and return 1
	try
	{
		print_ascii_header();
		Config config(file_name);

		Server server(config, ev);
		server.init();
	}
	catch(const std::exception& e)
	{
		std::cerr <<  "\033[1;31m" << "Error: " << e.what() << "\033[0m" << std::endl;
		return 1;
	}
	return 0;
}

