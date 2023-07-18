#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.hh"
#include "handler.hh"

#define PORT "6369"
#define BUF_SIZE 512
#define MAX_EVENTS 10

int main(int argc, char *argv[]) {
  struct epoll_event ev, events[MAX_EVENTS];
  int status, listener, epollfd, nfds;
  struct addrinfo hints, *res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;       // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
  hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

  if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (listener == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (bind(listener, res->ai_addr, res->ai_addrlen) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }
  freeaddrinfo(res);

  if (listen(listener, SOMAXCONN) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  printf("Listening on port %s\n", PORT);

  epollfd = epoll_create1(0);
  if (epollfd == -1) {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }

  ev.events = EPOLLIN;
  ev.data.fd = listener;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) == -1) {
    perror("epoll_ctl: listener");
    exit(EXIT_FAILURE);
  }

  // Keyspace holds all key-value pairs
  Keyspace ks;

  // The event loop
  while (1) {
    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1) {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nfds; i++) {
      if (events[i].events & EPOLLIN && events[i].data.fd == listener) {
        // We got a new connection! Add it to epoll interest list
        int newfd;
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof(client_addr);
        newfd = accept(listener, (struct sockaddr *)&client_addr, &addr_size);
        if (newfd == -1) {
          perror("accept");
          continue;
        }
        ev.events = EPOLLIN;
        ev.data.fd = newfd;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, newfd, &ev) == -1) {
          perror("epoll_ctl: accept");
          exit(EXIT_FAILURE);
        }
        struct sockaddr_in *sa = (struct sockaddr_in *)&client_addr;
        char ip4[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sa->sin_addr, ip4, INET_ADDRSTRLEN);
        printf("Accepted new connection: (%s, %d)\n", ip4, sa->sin_port);
      } else if (events[i].events & EPOLLIN) {
        // A connected client sent a command! Handle it
        char buf[BUF_SIZE], resp[BUF_SIZE];
        int bytes_read, bytes_sent;
        bytes_read = recv(events[i].data.fd, buf, BUF_SIZE, 0);
        if (bytes_read == -1) {
          perror("recv");
          exit(EXIT_FAILURE);
        }
        if (bytes_read == 0) {
          printf("Client disconnected\n");
          close(events[i].data.fd);
          continue;
        }
        auto cmd = parse_cmd(buf);
        handle_cmd(ks, cmd.get(), resp, BUF_SIZE);
        bytes_sent = send(events[i].data.fd, resp, strlen(resp), 0);
        if (bytes_sent == -1) {
          perror("send");
          exit(EXIT_FAILURE);
        }
      } else if (events[i].events & EPOLLERR) {
        perror("epoll_wait returned EPOLLERR");
        exit(EXIT_FAILURE);
      }
    }
  }
}
