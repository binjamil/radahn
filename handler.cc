#include <cstdio>
#include <cstring>
#include <map>
#include <string>

#include "protocol.hh"
#include "handler.hh"

typedef struct RadahnObject {
  char *val;
  int exp;
} RadahnObject;

static std::map<std::string, RadahnObject> keyspace;
static void ping(Cmd *cmd, char *resp, int resp_len);
static void get(Cmd *cmd, char *resp, int resp_len);
static void set(Cmd *cmd, char *resp, int resp_len);
static void del(Cmd *cmd, char *resp, int resp_len);

/************************/
/*** PUBLIC FUNCTIONS ***/
/************************/

void handle_cmd(Cmd *cmd, char *resp, int resp_len) {
  if (cmd->type == CmdTypePing) {
    ping(cmd, resp, resp_len);
  } else if (cmd->type == CmdTypeGet) {
    get(cmd, resp, resp_len);
  } else if (cmd->type == CmdTypeSet) {
    set(cmd, resp, resp_len);
  } else if (cmd->type == CmdTypeDel) {
    del(cmd, resp, resp_len);
  } else if (cmd->type == CmdTypeCommand) {
    sprintf(resp, "+OK\r\n");
  } else {
    sprintf(resp, "-ERR: Unsupported command\r\n");
  }
}

/*************************/
/*** PRIVATE FUNCTIONS ***/
/*************************/

static void ping(Cmd *cmd, char *resp, int resp_len) {
  if (cmd->argc == 1) {
    sprintf(resp, "+PONG\r\n");
  } else {
    char *pong = cmd->argv[1];
    sprintf(resp, "$%lu\r\n%s\r\n", strlen(pong), pong);
  }
}

static void get(Cmd *cmd, char *resp, int resp_len) {
  if (cmd->argc < 2) {
    sprintf(resp, "-ERR: Not enough arguments\r\n");
  } else {
    std::string key = cmd->argv[1];
    auto it = keyspace.find(key);
    if (it == keyspace.end()) {
      sprintf(resp, "$-1\r\n");
      return;
    }
    RadahnObject obj = it->second;
    sprintf(resp, "$%lu\r\n%s\r\n", strlen(obj.val), obj.val);
  }
}

static void set(Cmd *cmd, char *resp, int resp_len) {
  if (cmd->argc < 3) {
    sprintf(resp, "-ERR: Not enough arguments\r\n");
  } else {
    std::string key = cmd->argv[1];
    RadahnObject obj;
    size_t val_len = strlen(cmd->argv[2]) + 1;
    char* val = (char *)calloc(val_len, sizeof(char));
    memcpy(val, cmd->argv[2], val_len);
    obj.val = val;
    obj.exp = -1;
    keyspace.insert_or_assign(key, obj);
    sprintf(resp, "+OK\r\n");
  }
}

static void del(Cmd *cmd, char *resp, int resp_len) {
  if (cmd->argc < 2) {
    sprintf(resp, "-ERR: Not enough arguments\r\n");
  } else {
    int n_del = 0;
    for (unsigned int i = 1; i < cmd->argc; i++) {
      std::string key = cmd->argv[i];
      n_del += keyspace.erase(key);
    }
    sprintf(resp, ":%d\r\n", n_del);
  }
}
