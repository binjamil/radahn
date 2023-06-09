#include "protocol.hh"
#include <cassert>
#include <cstdio>
#include <cstring>
// parse_cmd returns CmdTypeInvalid for invalid command

int main(int argc, char *argv[]) {
  const char *buf = "*1\r\n$6\r\nBLARGH\r\n";
  Cmd *cmd = parse_cmd((char *)buf);
  assert(cmd->type == CmdTypeInvalid);
  assert(cmd->argc == 1);
  assert(strcmp(cmd->argv[0], "BLARGH") == 0);
  cleanup_cmd(cmd);
  printf("%s passed!\n", argv[0]);
}
