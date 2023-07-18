#include "protocol.hh"
#include <cassert>
#include <cstdio>
#include <cstring>
// parse_cmd returns CmdTypeInvalid for invalid command

int main(int argc, char *argv[]) {
  std::string buf{"*1\r\n$6\r\nBLARGH\r\n"};
  auto cmd = parse_cmd(buf.c_str());
  assert(cmd->type == CmdTypeInvalid);
  assert(cmd->argc == 1);
  assert(cmd->argv[0] == "BLARGH");
  printf("%s passed!\n", argv[0]);
}
