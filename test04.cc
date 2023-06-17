#include "protocol.hh"
#include <cassert>
#include <cstdio>
#include <cstring>
// parse_cmd correctly parses SET with options

int main(int argc, char *argv[]) {
  const char *buf = "*5\r\n$3\r\nSET\r\n$6\r\nsheesh\r\n$5\r\nmahal\r\n$2\r\nEX\r\n$2\r\n10\r\n";
  Cmd *cmd = parse_cmd((char *)buf);
  assert(cmd->type == CmdTypeSet);
  assert(cmd->argc == 5);
  assert(cmd->argv);
  assert(strcmp(cmd->argv[0], "SET") == 0);
  assert(strcmp(cmd->argv[1], "sheesh") == 0);
  assert(strcmp(cmd->argv[2], "mahal") == 0);
  assert(strcmp(cmd->argv[3], "EX") == 0);
  assert(strcmp(cmd->argv[4], "10") == 0);
  cleanup_cmd(cmd);
  printf("%s passed!\n", argv[0]);
}
