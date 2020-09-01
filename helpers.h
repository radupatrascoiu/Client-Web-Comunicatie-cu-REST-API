#ifndef UTILS_H
#define UTILS_H

#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <locale>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

#define SERVER_PORT 8080
#define BUFLEN 4096
#define LINELEN 1000

using namespace std;

typedef struct {
  char *data;
  size_t size;
} buffer;

buffer buffer_init(void);

void buffer_destroy(buffer *buffer);

void buffer_add(buffer *buffer, const char *data, size_t data_size);

int buffer_is_empty(buffer *buffer);

int buffer_find(buffer *buffer, const char *data, size_t data_size);

int buffer_find_insensitive(buffer *buffer, const char *data, size_t data_size);

void error(const char *msg);

void compute_message(char *message, const char *line);

int open_connection(char *host_ip, int portno, int ip_type, int socket_type,
                    int flag);

void close_connection(int sockfd);

void send_to_server(int sockfd, char *message);

char *receive_from_server(int sockfd);

char *basic_extract_json_response(char *str);

char *compute_get_request(const char *host, const char *url, char *query_params,
                          string cookies[50], int cookies_count);

char *compute_post_request(const char *host, const char *url,
                           const char *content_type, string body_data,
                           int body_data_fields_count, string cookies[50],
                           int cookies_count);

char *compute_delete_request(const char *host, const char *url,
                             char *query_params, string cookies[50],
                             int cookies_count);

#endif