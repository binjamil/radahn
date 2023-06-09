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
  cmd->argv = (char**) calloc(arglen, sizeof(char*));

  for (int i=0; i<arglen; i++) {
    int tokenlen = 0;
    buf += 3;
    while (*buf != '\r') {
      tokenlen = (tokenlen * 10) + (*buf - '0');
      buf++;
    }
    buf += 2;
    cmd->argv[i] = (char*) calloc(tokenlen + 1, sizeof(char));
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
  return CmdTypeInvalid;
}

// char *s1 = "PING";
// char *s2 = "SHEESHKABABPAKSORA";
// printf("%lu\n", sizeof(Cmd));
// Cmd* cmd = (Cmd*) malloc(sizeof(Cmd));
// memset(cmd, 0, sizeof(Cmd));
// cmd->type = CmdTypePing;
// cmd->argc = 2;
// cmd->argv = (char**) calloc(cmd->argc, sizeof(char*));
// printf("cmd_type: %d\ncmd_argc: %d\ncmd_args: %p\n", cmd->type, cmd->argc,
// cmd->argv); for (int i = 0; i < cmd->argc; i++) {
//   char* s;
//   if (i == 0) {
//     s = s1;
//   } else {
//     s = s2;
//   }
//   cmd->argv[i] = (char*) calloc(strlen(s) + 1, 1);
//   memcpy(cmd->argv[i], s, strlen(s));
// }
// for (int i = 0; i < cmd->argc; i++) {
//   printf("cmd_argv[%d]: %s\n", i, cmd->argv[i]);
// }
//
// for (int i = 0; i < cmd->argc; i++) {
//   free(cmd->argv[i]);
// }
// free(cmd->argv);
// free(cmd);
// return 0;
