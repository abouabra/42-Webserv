#pragma once


#define BUFFER_SIZE 1024
#define SELECT_TIMEOUT 10
#define REQUEST_TIMEOUT 30

#define CGI_TIMEOUT 30

#define DEFAULT_MAX_BODY_SIZE 1000000
#define DEFAULT_INDEX_FILE "index.html"
#define DEFAULT_PORT 8080
#define DEFAULT_HOST "127.0.0.1"

#ifndef DEFAULT_ROOT
	#define DEFAULT_ROOT "/Users/abouabra/1337/backserve/assets/server_1/"
#endif

#ifndef DEFAULT_CONFIG
	#define DEFAULT_CONFIG "/Users/abouabra/1337/backserve/config/default.conf"
#endif