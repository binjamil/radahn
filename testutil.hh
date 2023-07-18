#ifndef __TESTUTIL_HH__
#define __TESTUTIL_HH__

#include "protocol.hh"
#include <string>

std::unique_ptr<Cmd> setup_cmd(CmdType t, int argc, std::string argv[]);

#endif
