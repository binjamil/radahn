#include "protocol.hh"
#include <cassert>
#include <cstdio>
#include <cstring>
// parse_cmd correctly parses PING

int main(int argc, char *argv[]) {
  std::string buf{"*2\r\n$4\r\nPING\r\n$6\r\nsheesh\r\n"};
  auto cmd = parse_cmd(buf.c_str());
  assert(cmd->type == CmdTypePing);
  assert(cmd->argc == 2);
  assert(cmd->argv.size() == 2);
  assert(cmd->argv[0] == "PING");
  assert(cmd->argv[1] == "sheesh");
  printf("%s passed!\n", argv[0]);
}
