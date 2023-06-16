#ifndef __TESTUTIL_HH__
#define __TESTUTIL_HH__

#include "protocol.hh"
#include <string>

Cmd* setup_cmd(CmdType t, std::string argv[]);
void teardown_cmd(Cmd* cmd);

#endif
