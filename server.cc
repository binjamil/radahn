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

#define MAXCONN 4096
#define PORT "6369"

struct client_info {
  int sock_fd;
  struct sockaddr_storage addr;
  pthread_t thread_id;
};

static void *client_handler(void *arg) {
  struct client_info *cinfo = (struct client_info *)arg;
  struct sockaddr_in *sa = (struct sockaddr_in *)&cinfo->addr;
  char ip4[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &sa->sin_addr, ip4, INET_ADDRSTRLEN);
  printf("Accepted new connection: (%s, %d)\n", ip4, sa->sin_port);

  close(cinfo->sock_fd);
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
    cinfo->sock_fd = client_fd;
    // cinfo->addr = client_addr;
    memcpy(&cinfo->addr, &client_addr, addr_size);
    pthread_create(&cinfo->thread_id, NULL, client_handler, (void *)cinfo);
  }
}
