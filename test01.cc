#include "protocol.hh"
#include <cassert>
#include <cstdio>
// parse_cmd returns CmdTypeInvalid for invalid starter token

int main(int argc, char *argv[]) {
  std::string buf{"PING"};
  auto cmd = parse_cmd(buf.c_str());
  assert(cmd->type == CmdTypeInvalid);
  assert(cmd->argc == 0);
  assert(cmd->argv.empty());
  printf("%s passed!\n", argv[0]);
}
