#ifndef __HANDLER_HH__
#define __HANDLER_HH__

#include <cstdint>
#include <ctime>
#include <map>
#include <string>

#include "protocol.hh"

typedef struct RadahnObject {
  std::string val;
  time_t exp;
} RadahnObject;

typedef std::map<std::string, RadahnObject> Keyspace;

void handle_cmd(Keyspace& keyspace, Cmd *cmd, char* resp, int resp_len);

#endif
