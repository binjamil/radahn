#include <cstdlib>
#include <cstring>

#include "protocol.hh"

static Cmd *_invalid_cmd(Cmd *cmd);

/************************/
/*** PUBLIC FUNCTIONS ***/
/************************/

Cmd *parse_cmd(char *buf) {
  Cmd *cmd = (Cmd *)malloc(sizeof(Cmd));
  memset(cmd, 0, sizeof(Cmd));

  if (*buf != '*') {
    return _invalid_cmd(cmd);
  }

  cmd->type = CmdTypePing;
  cmd->argc = 0;
  cmd->argv = nullptr;
  return cmd;
}

/*************************/
/*** PRIVATE FUNCTIONS ***/
/*************************/

static Cmd *_invalid_cmd(Cmd *cmd) {
  cmd->type = CmdTypeInvalid;
  cmd->argc = 0;
  cmd->argv = nullptr;
  return cmd;
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
