#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.hh"

#define MAXCONN 4096
#define PORT "6369"
#define BUF_SIZE 512

struct client_info {
  int sockfd;
  struct sockaddr_storage addr;
  pthread_t thread_id;
};

static void *client_handler(void *arg) {
  struct client_info *cinfo = (struct client_info *)arg;
  struct sockaddr_in *sa = (struct sockaddr_in *)&cinfo->addr;
  char ip4[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &sa->sin_addr, ip4, INET_ADDRSTRLEN);
  printf("Accepted new connection: (%s, %d)\n", ip4, sa->sin_port);

  char buf[BUF_SIZE], resp[BUF_SIZE];
  int bytes_read, bytes_sent;
  while (1) {
    bytes_read = recv(cinfo->sockfd, buf, BUF_SIZE, 0);
    if (bytes_read == -1) {
      perror("recv");
    }
    if (bytes_read == 0) {
      break;
    }

    Cmd *cmd = parse_cmd(buf);
    // TODO: move this to handler.cc
    if (cmd->type == CmdTypePing) {
      if (cmd->argc == 1) {
        sprintf(resp, "+PONG\r\n");
      } else {
        char *pong = cmd->argv[1];
        sprintf(resp, "$%lu\r\n%s\r\n", strlen(pong), pong);
      }
      bytes_sent = send(cinfo->sockfd, resp, strlen(resp), 0);
      if (bytes_sent == -1) {
        perror("send");
      }
    } else {
      sprintf(resp, "+OK\r\n");
      bytes_sent = send(cinfo->sockfd, resp, strlen(resp), 0);
      if (bytes_sent == -1) {
        perror("send");
      }
    }
    cleanup_cmd(cmd);
  }

  printf("Connection closed: (%s, %d)\n", ip4, sa->sin_port);
  close(cinfo->sockfd);
  free(cinfo);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  int status, sockfd;
  struct addrinfo hints, *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;       // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
  hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

  if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sockfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }
  freeaddrinfo(res);

  if (listen(sockfd, MAXCONN) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  printf("Listening on port %s\n", PORT);

  while (1) {
    int client_fd;
    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof(client_addr);

    client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
    if (client_fd == -1) {
      perror("accept");
      continue;
    }

    size_t sz = sizeof(struct client_info);
    struct client_info *cinfo = (struct client_info *)malloc(sz);
    memset(cinfo, 0, sz);
    cinfo->sockfd = client_fd;
    // cinfo->addr = client_addr;
    memcpy(&cinfo->addr, &client_addr, addr_size);
    pthread_create(&cinfo->thread_id, NULL, client_handler, (void *)cinfo);
  }
}
