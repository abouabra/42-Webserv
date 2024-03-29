#include "../includes/Config.hpp"
#include "../includes/Utils.hpp"

/*
	this is the main function that will be called when the program is executed
	it will check if the user has provided a config file as an argument
	if not, it will use the default config file
	it will then read, parses, checks and then creates the Config object
*/

int main(int ac, char **av)
{
	std::string file_name;

	if(ac > 2)
	{
		std::cout << "Usage: ./webserv <config_file>" << std::endl;
		return 1;
	}
	if(ac == 2)
		file_name = av[1];
	else
		file_name = "./config/new.conf";
	try
	{
		Config config(file_name);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
