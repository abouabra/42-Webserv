#include "../includes/Config.hpp"
#include "../includes/Utils.hpp"
#include "../includes/Server.hpp"


int main(int ac, char **av, char **ev)
{
	std::string file_name;

	if(ac > 2)
	{
		std::cerr << "Usage: ./webserv <config_file>" << std::endl;
		return 1;
	}

	if(ac == 2)
		file_name = av[1];
	else
		file_name = "./config/Default.conf";

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

