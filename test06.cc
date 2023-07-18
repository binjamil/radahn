#include "handler.hh"
#include "testutil.hh"
#include <cassert>
#include <cstdio>
#include <cstring>
// hanlde_cmd correctly handles SET

int main(int argc, char *argv[]) {
  std::string test_argv[3]{"SET", "foo", "bar"};
  auto cmd = setup_cmd(CmdTypeSet, 3, test_argv);
  Keyspace ks;

  char buf[512];
  handle_cmd(ks, cmd.get(), buf, 512);
  assert(strcmp(buf, "+OK\r\n") == 0);
  assert(ks.size() == 1);
  auto it = ks.find(test_argv[1]);
  assert(it->first == test_argv[1]);
  assert(it->second.val == test_argv[2]);
  printf("%s passed!\n", argv[0]);
}
