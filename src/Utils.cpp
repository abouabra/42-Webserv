#include "../includes/Utils.hpp"
#include <cstddef>
#include <iostream>

void log(std::string message, int color)
{
	time_t current;
	char buffer[100];

	current = std::time(NULL);
	std::memset(buffer, 0, sizeof(buffer));
	std::strftime(buffer, 100, "[%d %b %Y %T]", std::localtime(&current));

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

	if(!file.is_open())
		throw std::runtime_error("Error: Could not open file: " + name);

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
		if(!std::isdigit(str[i]))
			return -1;
		num = num * 10 + (str[i] - '0');
	}
	return num * sign;
}


int ip_to_int(std::string &host)
{
	// Example: "127.0.0.1" -> 2130706433
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
	// Example: 2130706433 -> "127.0.0.1"
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


std::string convert_size(size_t size)
{
	// we will convert the size to the most appropriate unit

	std::string unit = "B";
	if (size > 1024)
	{
		size /= 1024;
		unit = "KB";
	}
	if (size > 1024)
	{
		size /= 1024;
		unit = "MB";
	}
	if (size > 1024)
	{
		size /= 1024;
		unit = "GB";
	}
	return itoa(size) + " " + unit;
}

std::string extension_to_html_icon(std::string extension) {

    // Map extensions to corresponding HTML character icons
    std::map<std::string, std::string> icons;

	icons["txt"] = "&#128196";  // Text file
	icons["html"] = "&#128195";  // HTML file
	icons["htm"] = "&#128195";   // HTM file
	icons["pdf"] = "&#128467";   // PDF document
	icons["doc"] = "&#128205";   // Word document
	icons["docx"] = "&#128205";  // Word document (newer format)
	icons["xls"] = "&#128185";   // Excel spreadsheet
	icons["xlsx"] = "&#128185";  // Excel spreadsheet (newer format)
	icons["ppt"] = "&#128221";   // PowerPoint presentation
	icons["pptx"] = "&#128221";  // PowerPoint presentation (newer format)
	icons["jpg"] = "&#128257";   // JPEG image
	icons["jpeg"] = "&#128257";  // JPEG image
	icons["png"] = "&#128254";   // PNG image
	icons["gif"] = "&#128195";   // GIF image
	icons["bmp"] = "&#128258";   // BMP image
	icons["mp3"] = "&#128245";   // MP3 audio
	icons["wav"] = "&#128259";   // WAV audio
	icons["mp4"] = "&#128249";   // MP4 video
 

    // Handle default case for unknown extensions
    std::string default_icon = "&#128196";  // Generic file icon

    if (icons.find(extension) != icons.end()) {
        // Found a specific mapping, return the associated icon
        return icons[extension];
    } else {
        // No specific mapping, return the default icon
        return default_icon;
    }
}

std::string GenerateUniqueFileName() {
    std::string tempFilename = "/tmp/webserv_";
	size_t result_length = 32;

    std::string alphanemeric = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int length = alphanemeric.length();

    size_t seed = std::time(NULL);

    for (size_t i = 0; i < result_length; ++i) {
        seed = (seed * 1103515245 + 12345) & 0x7fffffff; // Linear congruential generator
        tempFilename += alphanemeric[seed % length];
    }

	tempFilename += ".tmp";
    
	return tempFilename;
}

std::string decode_URL(std::string URL) {
    std::string decoded_URL;
    decoded_URL.reserve(URL.length());  // Optimization: Reserve capacity

    for (size_t i = 0; i < URL.length(); ++i) {
		if (URL[i] == '%' && i + 2 < URL.length()) {
            char hex1 = URL[i + 1];
            char hex2 = URL[i + 2];
            if (std::isxdigit(hex1) && std::isxdigit(hex2)) {
		
				// Convert hexadecimal digits to integer value
				int digit1 = std::isdigit(hex1) ? hex1 - '0' : std::toupper(hex1) - 'A' + 10;
				int digit2 = std::isdigit(hex2) ? hex2 - '0' : std::toupper(hex2) - 'A' + 10;
				char decodedChar = (digit1 << 4) + digit2;
				
                decoded_URL += decodedChar;
                i += 2; // Skip the next two characters
            } else {
                decoded_URL += URL[i];
            }
        }
		else
            decoded_URL += URL[i];
    }
    return decoded_URL;
}

char* my_strdup(const std::string& str)
{
  size_t len = str.length() + 1;  // Allocate space for null terminator
  char* arg = new char[len];
  std::strcpy(arg, str.c_str());  // Copy string content
  return arg;
}