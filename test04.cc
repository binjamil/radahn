#include "protocol.hh"
#include <cassert>
#include <cstdio>
#include <cstring>
// parse_cmd correctly parses SET with options

int main(int argc, char *argv[]) {
  std::string buf{"*5\r\n$3\r\nSET\r\n$6\r\nsheesh\r\n$5\r\nmahal\r\n$2\r\nEX\r\n$2\r\n10\r\n"};
  auto cmd = parse_cmd(buf.c_str());
  assert(cmd->type == CmdTypeSet);
  assert(cmd->argc == 5);
  assert(cmd->argv.size() == 5);
  assert(cmd->argv[0] == "SET");
  assert(cmd->argv[1] == "sheesh");
  assert(cmd->argv[2] == "mahal");
  assert(cmd->argv[3] == "EX");
  assert(cmd->argv[4] == "10");
  printf("%s passed!\n", argv[0]);
}
