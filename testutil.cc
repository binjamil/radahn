#include "testutil.hh"

Cmd* setup_cmd(CmdType t, std::string argv[]) {
  Cmd *cmd = (Cmd *)calloc(1, sizeof(Cmd));
  cmd->type = CmdTypeGet;
  cmd->argc = 2;
  cmd->argv = (char **)calloc(cmd->argc, sizeof(char *));
  for (int i = 0; i < 2; i++) {
    cmd->argv[i] = (char *)argv[i].c_str();
  }
  return cmd;
}

void teardown_cmd(Cmd* cmd) {
  free(cmd->argv);
  free(cmd);
}
