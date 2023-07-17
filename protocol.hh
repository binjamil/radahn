#ifndef __PROTOCOL_HH__
#define __PROTOCOL_HH__

#include <cstdint>
#include <string>
#include <vector>

enum CmdType {
  CmdTypeInvalid,
  CmdTypePing,
  CmdTypeCommand,
  CmdTypeGet,
  CmdTypeSet,
  CmdTypeDel,
};

typedef struct {
  CmdType type;
  unsigned int argc;
  // char **argv;
  std::vector<std::string> argv;
} Cmd;

Cmd *parse_cmd(char *buf);
void cleanup_cmd(Cmd *cmd);

#endif
