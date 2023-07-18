#include "handler.hh"
#include "testutil.hh"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
// hanlde_cmd correctly handles DEL

int main(int argc, char *argv[]) {
  std::string test_argv[2] = {"DEL", "foo"};
  std::string test_val{"bar"};

  auto cmd = setup_cmd(CmdTypeDel, 2, test_argv);
  Keyspace ks;
  RadahnObject robj;
  robj.val = test_val;
  robj.exp = -1;
  ks[test_argv[1]] = robj;

  char buf[512];
  handle_cmd(ks, cmd.get(), buf, 512);
  assert(strcmp(buf, ":1\r\n") == 0);
  assert(ks.size() == 0);
  auto it = ks.find(test_argv[1]);
  assert(it == ks.end());
  printf("%s passed!\n", argv[0]);
}
