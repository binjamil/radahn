#include <cstdlib>
#include <cstring>

#include "protocol.hh"

static CmdType getCmdType(std::string cmd_name);

/************************/
/*** PUBLIC FUNCTIONS ***/
/************************/

std::unique_ptr<Cmd> parse_cmd(const char *buf) {
  auto cmd{std::make_unique<Cmd>()};
  if (*buf != '*') {
    cmd->type = CmdTypeInvalid;
    return cmd;
  }

  int arglen = 0;
  buf++;
  while (*buf != '\r') {
    arglen = (arglen * 10) + (*buf - '0');
    buf++;
  }

  cmd->argc = arglen;

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
    buf += tokenlen;
  }

  if ((cmd->type = getCmdType(cmd->argv[0])) == CmdTypeInvalid) {
    cmd->type = CmdTypeInvalid;
    return cmd;
  }
  return cmd;
}

/*************************/
/*** PRIVATE FUNCTIONS ***/
/*************************/

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
