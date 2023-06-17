#include "handler.hh"
#include "testutil.hh"
#include <cassert>
#include <cstdio>
#include <cstring>
// hanlde_cmd correctly handles SET

int main(int argc, char *argv[]) {
  std::string test_argv[3] = {"SET", "foo", "bar"};
  Cmd *cmd = setup_cmd(CmdTypeSet, 3, test_argv);
  Keyspace ks;

  char buf[512];
  handle_cmd(ks, cmd, buf, 512);
  assert(strcmp(buf, "+OK\r\n") == 0);
  assert(ks.size() == 1);
  auto it = ks.find(test_argv[1]);
  assert(strcmp(it->first.c_str(), test_argv[1].c_str()) == 0);
  assert(strcmp(it->second.val, test_argv[2].c_str()) == 0);
  teardown_cmd(cmd);
  printf("%s passed!\n", argv[0]);
}
