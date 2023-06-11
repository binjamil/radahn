#include <cstdio>
#include <cstring>

#include "handler.hh"

void handle_cmd(Cmd *cmd, char *resp, int resp_len) {
  if (cmd->type == CmdTypePing) {
    if (cmd->argc == 1) {
      sprintf(resp, "+PONG\r\n");
    } else {
      char *pong = cmd->argv[1];
      sprintf(resp, "$%lu\r\n%s\r\n", strlen(pong), pong);
    }
  } else {
    sprintf(resp, "+OK\r\n");
  }
}
