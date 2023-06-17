#include "testutil.hh"

Cmd* setup_cmd(CmdType t, int argc, std::string argv[]) {
  Cmd *cmd = (Cmd *)calloc(1, sizeof(Cmd));
  cmd->type = t;
  cmd->argc = argc;
  cmd->argv = (char **)calloc(argc, sizeof(char *));
  for (int i = 0; i < argc; i++) {
    cmd->argv[i] = (char *)argv[i].c_str();
  }
  return cmd;
}

void teardown_cmd(Cmd* cmd) {
  free(cmd->argv);
  free(cmd);
}
