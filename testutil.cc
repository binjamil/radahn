#include "testutil.hh"

std::unique_ptr<Cmd> setup_cmd(CmdType t, int argc, std::string argv[]) {
  auto cmd{std::make_unique<Cmd>()};
  cmd->type = t;
  cmd->argc = argc;
  for (int i = 0; i < argc; i++) {
    cmd->argv.push_back(argv[i]);
  }
  return cmd;
}
