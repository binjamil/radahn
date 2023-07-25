#ifndef __TASKQUEUE_HH__
#define __TASKQUEUE_HH__

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

#include "protocol.hh"

typedef struct Task {
  std::shared_ptr<Cmd> cmd;
  int fd;
} Task;

typedef struct Queue {
  std::mutex mut;
  std::queue<Task> queue;
  std::condition_variable cond;
} Queue;

std::shared_ptr<Queue> create_queue();
void push(Queue *q, Task t);
std::shared_ptr<Task> wait_and_pop(Queue *q);

#endif
