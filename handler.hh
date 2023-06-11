#ifndef __HANDLER_HH__
#define __HANDLER_HH__

#include <cstdint>
#include "protocol.hh"

void handle_cmd(Cmd *cmd, char* resp, int resp_len);

#endif
