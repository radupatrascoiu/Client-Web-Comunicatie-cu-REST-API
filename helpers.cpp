#include "helpers.h"

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

void error(const char *msg) {
  perror(msg);
  exit(0);
}

void compute_message(char *message, const char *line) {
  strcat(message, line);
  strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type,
                    int flag) {
  struct sockaddr_in serv_addr;
  int sockfd = socket(ip_type, socket_type, flag);
  if (sockfd < 0) error("ERROR opening socket");

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = ip_type;
  serv_addr.sin_port = htons(portno);
  inet_aton(host_ip, &serv_addr.sin_addr);

  /* connect the socket */
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR connecting");

  return sockfd;
}

void close_connection(int sockfd) { close(sockfd); }

void send_to_server(int sockfd, char *message) {
  int bytes, sent = 0;
  int total = strlen(message);

  do {
    bytes = write(sockfd, message + sent, total - sent);
    if (bytes < 0) {
      error("ERROR writing message to socket");
    }

    if (bytes == 0) {
      break;
    }

    sent += bytes;
  } while (sent < total);
}

char *receive_from_server(int sockfd) {
  char response[BUFLEN];
  buffer buffer = buffer_init();
  int header_end = 0;
  int content_length = 0;

  do {
    int bytes = read(sockfd, response, BUFLEN);

    if (bytes < 0) {
      error("ERROR reading response from socket");
    }

    if (bytes == 0) {
      break;
    }

    buffer_add(&buffer, response, (size_t)bytes);

    header_end =
        buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

    if (header_end >= 0) {
      header_end += HEADER_TERMINATOR_SIZE;

      int content_length_start =
          buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);

      if (content_length_start < 0) {
        continue;
      }

      content_length_start += CONTENT_LENGTH_SIZE;
      content_length = strtol(buffer.data + content_length_start, NULL, 10);
      break;
    }
  } while (1);
  size_t total = content_length + (size_t)header_end;

  while (buffer.size < total) {
    int bytes = read(sockfd, response, BUFLEN);

    if (bytes < 0) {
      error("ERROR reading response from socket");
    }

    if (bytes == 0) {
      break;
    }

    buffer_add(&buffer, response, (size_t)bytes);
  }
  buffer_add(&buffer, "", 1);
  return buffer.data;
}

char *basic_extract_json_response(char *str) { return strstr(str, "{\""); }

buffer buffer_init(void) {
  buffer buffer;

  buffer.data = NULL;
  buffer.size = 0;

  return buffer;
}

void buffer_destroy(buffer *buffer) {
  if (buffer->data != NULL) {
    free(buffer->data);
    buffer->data = NULL;
  }

  buffer->size = 0;
}

int buffer_is_empty(buffer *buffer) { return buffer->data == NULL; }

void buffer_add(buffer *buffer, const char *data, size_t data_size) {
  if (buffer->data != NULL) {
    buffer->data = (char *)realloc(buffer->data,
                                   (buffer->size + data_size) * sizeof(char));
  } else {
    buffer->data = (char *)calloc(data_size, sizeof(char));
  }

  memcpy(buffer->data + buffer->size, data, data_size);

  buffer->size += data_size;
}

int buffer_find(buffer *buffer, const char *data, size_t data_size) {
  if (data_size > buffer->size) return -1;

  size_t last_pos = buffer->size - data_size + 1;

  for (size_t i = 0; i < last_pos; ++i) {
    size_t j;

    for (j = 0; j < data_size; ++j) {
      if (buffer->data[i + j] != data[j]) {
        break;
      }
    }

    if (j == data_size) return i;
  }

  return -1;
}

int buffer_find_insensitive(buffer *buffer, const char *data,
                            size_t data_size) {
  if (data_size > buffer->size) return -1;

  size_t last_pos = buffer->size - data_size + 1;

  for (size_t i = 0; i < last_pos; ++i) {
    size_t j;

    for (j = 0; j < data_size; ++j) {
      if (tolower(buffer->data[i + j]) != tolower(data[j])) {
        break;
      }
    }

    if (j == data_size) return i;
  }

  return -1;
}

char *compute_post_request(const char *host, const char *url,
                           const char *content_type, string body_data,
                           int body_data_fields_count, string cookies[50],
                           int cookies_count) {
  char *message = (char *)calloc(BUFLEN, sizeof(char));
  char *line = (char *)calloc(LINELEN, sizeof(char));
  int content_len = 0;

  content_len = body_data.length();

  sprintf(line, "POST %s HTTP/1.1", url);
  compute_message(message, line);

  sprintf(line, "Host: %s", host);
  compute_message(message, line);

  sprintf(line, "Content-Type: %s", content_type);
  compute_message(message, line);

  sprintf(line, "Content-Length: %d", content_len);
  compute_message(message, line);

  if (cookies != NULL) {
    for (int i = 0; i < cookies_count; i++) {
      compute_message(message, cookies[i].c_str());
    }
  }

  compute_message(message, "");

  memset(line, 0, LINELEN);
  if (body_data_fields_count > 0) {
    compute_message(message, body_data.c_str());
  }

  free(line);
  return message;
}

char *compute_get_request(const char *host, const char *url, char *query_params,
                          string cookies[50], int cookies_count) {
  char *message = (char *)calloc(BUFLEN, sizeof(char));
  char *line = (char *)calloc(LINELEN, sizeof(char));
  int i;

  if (query_params != NULL) {
    sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
  } else {
    sprintf(line, "GET %s HTTP/1.1", url);
  }

  compute_message(message, line);

  sprintf(line, "Host: %s", host);
  compute_message(message, line);

  if (cookies != NULL) {
    for (i = 0; i < cookies_count; i++) {
      compute_message(message, cookies[i].c_str());
    }
  }
  compute_message(message, "");
  return message;
}

char *compute_delete_request(const char *host, const char *url,
                             char *query_params, string cookies[50],
                             int cookies_count) {
  char *message = (char *)calloc(BUFLEN, sizeof(char));
  char *line = (char *)calloc(LINELEN, sizeof(char));
  int i;

  if (query_params != NULL) {
    sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
  } else {
    sprintf(line, "DELETE %s HTTP/1.1", url);
  }

  compute_message(message, line);

  sprintf(line, "Host: %s", host);
  compute_message(message, line);

  if (cookies != NULL) {
    for (i = 0; i < cookies_count; i++) {
      compute_message(message, cookies[i].c_str());
    }
  }
  compute_message(message, "");
  return message;
}