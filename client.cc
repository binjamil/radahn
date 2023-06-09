#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SERVER_PORT "6369"

int main(int argc, char *argv[]) {
  int status, sockfd;
  struct addrinfo hints, *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;       // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

  if ((status = getaddrinfo("0.0.0.0", SERVER_PORT, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sockfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  printf("Connection established\n");

  char *cmd = "*2\r\n$4\r\nPING\r\n$6\r\nsheesh\r\n";
  int len, bytes_sent;

  len = strlen(cmd);
  bytes_sent = send(sockfd, cmd, len, 0);
  if (bytes_sent == -1) {
    perror("send");
    exit(EXIT_FAILURE);
  }
  if (bytes_sent != len) {
    fprintf(stderr, "Failed to send complete data in one go!");
    exit(EXIT_FAILURE);
  }
  printf("send: %s\n", cmd);

  freeaddrinfo(res);
}
