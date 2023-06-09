#include "protocol.hh"
#include <cassert>
#include <cstdio>
#include <cstring>
// parse_cmd correctly parses PING

int main(int argc, char *argv[]) {
  const char *buf = "*2\r\n$4\r\nPING\r\n$6\r\nsheesh\r\n";
  Cmd *cmd = parse_cmd((char *)buf);
  assert(cmd->type == CmdTypePing);
  assert(cmd->argc == 2);
  assert(cmd->argv);
  assert(strcmp(cmd->argv[0], "PING") == 0);
  assert(strcmp(cmd->argv[1], "sheesh") == 0);
  cleanup_cmd(cmd);
  printf("%s passed!\n", argv[0]);
}
