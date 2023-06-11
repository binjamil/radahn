#include <cstdlib>
#include <cstring>

#include "protocol.hh"

static Cmd *_invalid_cmd(Cmd *cmd);
static CmdType getCmdType(char *cmd_name);

/************************/
/*** PUBLIC FUNCTIONS ***/
/************************/

Cmd *parse_cmd(char *buf) {
  Cmd *cmd = (Cmd *)malloc(sizeof(Cmd));
  memset(cmd, 0, sizeof(Cmd));
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
  cmd->argv = (char **)calloc(arglen, sizeof(char *));

  for (int i = 0; i < arglen; i++) {
    int tokenlen = 0;
    buf += 3;
    while (*buf != '\r') {
      tokenlen = (tokenlen * 10) + (*buf - '0');
      buf++;
    }
    buf += 2;
    cmd->argv[i] = (char *)calloc(tokenlen + 1, sizeof(char));
    memcpy(cmd->argv[i], buf, tokenlen);
    cmd->argv[i][tokenlen] = '\0';
    buf += tokenlen;
  }

  if ((cmd->type = getCmdType(cmd->argv[0])) == CmdTypeInvalid) {
    return _invalid_cmd(cmd);
  }
  return cmd;
}

void cleanup_cmd(Cmd *cmd) {
  for (unsigned int i = 0; i < cmd->argc; i++) {
    free(cmd->argv[i]);
  }
  if (cmd->argv) {
    free(cmd->argv);
  }
  free(cmd);
}

/*************************/
/*** PRIVATE FUNCTIONS ***/
/*************************/

static Cmd *_invalid_cmd(Cmd *cmd) {
  cmd->type = CmdTypeInvalid;
  return cmd;
}

static CmdType getCmdType(char *cmd_name) {
  if (strcmp(cmd_name, "PING") == 0) {
    return CmdTypePing;
  }
  if (strcmp(cmd_name, "COMMAND") == 0) {
    return CmdTypeCommand;
  }
  if (strcmp(cmd_name, "GET") == 0) {
    return CmdTypeGet;
  }
  if (strcmp(cmd_name, "SET") == 0) {
    return CmdTypeSet;
  }
  if (strcmp(cmd_name, "DEL") == 0) {
    return CmdTypeDel;
  }
  return CmdTypeInvalid;
}
