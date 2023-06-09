#include "protocol.hh"
#include <cassert>
#include <cstdio>
// parse_cmd returns CmdTypeInvalid for invalid commands

int main(int argc, char *argv[]) {
  const char *buf = "blargh";
  Cmd *cmd = parse_cmd((char *)buf);
  assert(cmd->type == CmdTypeInvalid);
  assert(cmd->argc == 0);
  assert(cmd->argv == nullptr);
  printf("%s passed!\n", argv[0]);
}
