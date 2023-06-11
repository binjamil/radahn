#ifndef __PROTOCOL_HH__
#define __PROTOCOL_HH__

#include <cstdint>

enum CmdType {
  CmdTypeInvalid,
  CmdTypePing,
  CmdTypeCommand,
  CmdTypeGet,
  CmdTypeSet,
  CmdTypeDel,
};

typedef struct Cmd {
  CmdType type;
  unsigned int argc;
  char **argv;
} Cmd;

Cmd *parse_cmd(char *buf);
void cleanup_cmd(Cmd *cmd);

#endif
