#include "protocol.hh"
#include <cassert>
#include <cstdio>
// parse_cmd returns CmdTypeInvalid for invalid starter token

int main(int argc, char *argv[]) {
  const char *buf = "PING";
  Cmd *cmd = parse_cmd((char *)buf);
  assert(cmd->type == CmdTypeInvalid);
  assert(cmd->argc == 0);
  assert(cmd->argv == nullptr);
  cleanup_cmd(cmd);
  printf("%s passed!\n", argv[0]);
}