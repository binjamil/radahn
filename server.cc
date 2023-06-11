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

#define MAXCONN 4096
#define PORT "6369"
#define BUF_SIZE 512
#define MAX_EVENTS 10

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

  if (listen(listener, MAXCONN) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  printf("Listening on port %s\n", PORT);

  // Create epoll file descriptor
  epollfd = epoll_create1(0);
  if (epollfd == -1) {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }

  // Add listener fd to epoll interest list
  ev.events = EPOLLIN;
  ev.data.fd = listener;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) == -1) {
    perror("epoll_ctl: listener");
    exit(EXIT_FAILURE);
  }

  // The event loop
  while (1) {
    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1) {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nfds; i++) {
      if (events[i].events & EPOLLERR) {
        perror("epoll_wait returned EPOLLERR");
        exit(EXIT_FAILURE);
      } else if (events[i].data.fd == listener && events[i].events & EPOLLIN) {
        // We got a new connection! Add it to epoll interest list
        int newfd;
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof(client_addr);
        newfd = accept(listener, (struct sockaddr *)&client_addr, &addr_size);
        if (newfd == -1) {
          perror("accept");
          continue;
        }
        struct epoll_event event;
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
          continue; // Maybe exit failure or remove fd from epoll
        }
        if (bytes_read == 0) {
          printf("Client disconnected during EPOLLIN\n");
          // printf("Connection closed: (%s, %d)\n", ip4, sa->sin_port);
          close(events[i].data.fd);
          continue;
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
          bytes_sent = send(events[i].data.fd, resp, strlen(resp), 0);
          if (bytes_sent == -1) {
            perror("send");
          }
        } else {
          sprintf(resp, "+OK\r\n");
          bytes_sent = send(events[i].data.fd, resp, strlen(resp), 0);
          if (bytes_sent == -1) {
            perror("send");
          }
        }
        cleanup_cmd(cmd);

      } else if (events[i].events & EPOLLHUP) {
        // A client disconnected maybe?
        printf("Client disconnected and fired EPOLLHUP event\n");
      }
    }
  }

  // while (1) {
  //   int client_fd;
  //   struct sockaddr_storage client_addr;
  //   socklen_t addr_size = sizeof(client_addr);
  //
  //   client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
  //   if (client_fd == -1) {
  //     perror("accept");
  //     continue;
  //   }
  //
  //   size_t sz = sizeof(struct client_info);
  //   struct client_info *cinfo = (struct client_info *)malloc(sz);
  //   memset(cinfo, 0, sz);
  //   cinfo->sockfd = client_fd;
  //   // cinfo->addr = client_addr;
  //   memcpy(&cinfo->addr, &client_addr, addr_size);
  //   pthread_create(&cinfo->thread_id, NULL, client_handler, (void *)cinfo);
  // }
}
