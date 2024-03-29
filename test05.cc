#include "handler.hh"
#include "testutil.hh"
#include <cassert>
#include <cstdio>
#include <cstring>
// hanlde_cmd correctly handles GET

int main(int argc, char *argv[]) {
  std::string test_argv[2]{"GET", "foo"};
  std::string test_val{"bar"};

  auto cmd = setup_cmd(CmdTypeGet, 2, test_argv);
  Keyspace ks;
  RadahnObject robj;
  robj.val = test_val;
  robj.exp = -1;
  ks[test_argv[1]] = robj;

  char buf[512];
  handle_cmd(ks, cmd.get(), buf, 512);
  assert(strcmp(buf, "$3\r\nbar\r\n") == 0);
  printf("%s passed!\n", argv[0]);
}
