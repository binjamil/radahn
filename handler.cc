#include <cstdio>
#include <cstring>

#include "handler.hh"

static void ping(Keyspace& keyspace, Cmd *cmd, char *resp, int resp_len);
static void get(Keyspace& keyspace, Cmd *cmd, char *resp, int resp_len);
static void set(Keyspace& keyspace, Cmd *cmd, char *resp, int resp_len);
static void del(Keyspace& keyspace, Cmd *cmd, char *resp, int resp_len);

/************************/
/*** PUBLIC FUNCTIONS ***/
/************************/

void handle_cmd(Keyspace& keyspace, Cmd *cmd, char* resp, int resp_len) {
  if (cmd->type == CmdTypePing) {
    ping(keyspace, cmd, resp, resp_len);
  } else if (cmd->type == CmdTypeGet) {
    get(keyspace, cmd, resp, resp_len);
  } else if (cmd->type == CmdTypeSet) {
    set(keyspace, cmd, resp, resp_len);
  } else if (cmd->type == CmdTypeDel) {
    del(keyspace, cmd, resp, resp_len);
  } else if (cmd->type == CmdTypeCommand) {
    sprintf(resp, "+OK\r\n");
  } else {
    sprintf(resp, "-ERR: Unsupported command\r\n");
  }
}

/*************************/
/*** PRIVATE FUNCTIONS ***/
/*************************/

static void ping(Keyspace& keyspace, Cmd *cmd, char *resp, int resp_len) {
  if (cmd->argc == 1) {
    sprintf(resp, "+PONG\r\n");
  } else {
    std::string pong = cmd->argv[1];
    sprintf(resp, "$%lu\r\n%s\r\n", pong.length(), pong.c_str());
  }
}

static void get(Keyspace& keyspace, Cmd *cmd, char *resp, int resp_len) {
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
    if (obj.exp != -1) {
      time_t now = time(NULL);
      if (now >= obj.exp) {
        // free(obj.val);
        keyspace.erase(it);
        sprintf(resp, "$-1\r\n");
        return;
      }
    }
    sprintf(resp, "$%lu\r\n%s\r\n", obj.val.length(), obj.val.c_str());
  }
}

static void set(Keyspace& keyspace, Cmd *cmd, char *resp, int resp_len) {
  if (cmd->argc < 3) {
    sprintf(resp, "-ERR: Not enough arguments\r\n");
  } else {
    time_t exp = -1;
    if (cmd->argc == 5) {
      if (cmd->argv[3] != "EX") {
        sprintf(resp, "-ERR: Unsupported command option\r\n");
        return;
      };
      int secs = atoi(cmd->argv[4].c_str());
      if (secs <= 0) {
        sprintf(resp, "-ERR: Invalid EX seconds value\r\n");
        return;
      }
      exp = time(NULL) + secs;
    }
    std::string key = cmd->argv[1];
    RadahnObject obj;
    std::string val = cmd->argv[2];
    // size_t val_len = strlen(cmd->argv[2]) + 1;
    // char *val = (char *)calloc(val_len, sizeof(char));
    // memcpy(val, cmd->argv[2], val_len);
    obj.val = val;
    obj.exp = exp;
    keyspace.insert_or_assign(key, obj);
    sprintf(resp, "+OK\r\n");
  }
}

static void del(Keyspace& keyspace, Cmd *cmd, char *resp, int resp_len) {
  if (cmd->argc < 2) {
    sprintf(resp, "-ERR: Not enough arguments\r\n");
  } else {
    int n_del = 0;
    for (unsigned int i = 1; i < cmd->argc; i++) {
      std::string key = cmd->argv[i];
      auto found = keyspace.find(key);
      if (found != keyspace.end()) {
        // free(found->second.val);
        keyspace.erase(found);
        n_del++;
      }
    }
    sprintf(resp, ":%d\r\n", n_del);
  }
}
