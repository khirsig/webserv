#pragma once

#define PRINT_LEVEL 0

#define MAX_CONNECTIONS 1024
#define TIMEOUT_TIME 20000

#define MAX_INFO_LEN 8196

#define MAX_PIPE_SIZE 1048576

#define FILE_BUF_SIZE 4096
#define CGI_BUF_SIZE 4096
#define CONNECTION_BUF_SIZE 4096

#define DEFAULT_CONFIG_FILE "./webserv.conf"

#define SERVER_NAME "webserv"

#define DIR_LISTING_CGI_PATH "/usr/bin/python3"
#define DIR_LISTING_CGI_SCRIPT_PATH "./cgi/directory_listing.py"

#define CLIENT_MAX_BODY_SIZE (1ULL << 32)  // 4GB
