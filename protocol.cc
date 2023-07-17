#include <cstdlib>
#include <cstring>

#include "protocol.hh"

static Cmd *_invalid_cmd(Cmd *cmd);
static CmdType getCmdType(std::string cmd_name);

/************************/
/*** PUBLIC FUNCTIONS ***/
/************************/

Cmd *parse_cmd(char *buf) {
  // Cmd *cmd = (Cmd *)malloc(sizeof(Cmd));
  // memset(cmd, 0, sizeof(Cmd));
  Cmd *cmd = new Cmd();
  if (*buf != '*') {
    return _invalid_cmd(cmd);
  }

  int arglen = 0;
  buf++;
  while (*buf != '\r') {
    arglen = (arglen * 10) + (*buf - '0');
    buf++;
  }

  cmd->argc = arglen;
  // cmd->argv = (char **)calloc(arglen, sizeof(char *));

  for (int i = 0; i < arglen; i++) {
    int tokenlen = 0;
    buf += 3;
    while (*buf != '\r') {
      tokenlen = (tokenlen * 10) + (*buf - '0');
      buf++;
    }
    buf += 2;
    std::string arg(buf, tokenlen);
    cmd->argv.push_back(arg);
    // cmd->argv[i] = (char *)calloc(tokenlen + 1, sizeof(char));
    // memcpy(cmd->argv[i], buf, tokenlen);
    // cmd->argv[i][tokenlen] = '\0';
    buf += tokenlen;
  }

  if ((cmd->type = getCmdType(cmd->argv[0])) == CmdTypeInvalid) {
    return _invalid_cmd(cmd);
  }
  return cmd;
}

void cleanup_cmd(Cmd *cmd) {
  // for (unsigned int i = 0; i < cmd->argc; i++) {
  //   free(cmd->argv[i]);
  // }
  // if (cmd->argv) {
  //   free(cmd->argv);
  // }
  // free(cmd);
  delete cmd;
}

/*************************/
/*** PRIVATE FUNCTIONS ***/
/*************************/

static Cmd *_invalid_cmd(Cmd *cmd) {
  cmd->type = CmdTypeInvalid;
  return cmd;
}

static CmdType getCmdType(std::string cmd_name) {
  if (cmd_name == "PING") {
    return CmdTypePing;
  }
  if (cmd_name == "COMMAND") {
    return CmdTypeCommand;
  }
  if (cmd_name == "GET") {
    return CmdTypeGet;
  }
  if (cmd_name == "SET") {
    return CmdTypeSet;
  }
  if (cmd_name == "DEL") {
    return CmdTypeDel;
  }
  return CmdTypeInvalid;
}
