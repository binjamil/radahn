#include "taskqueue.hh"
#include <memory>

/************************/
/*** PUBLIC FUNCTIONS ***/
/************************/

std::shared_ptr<Queue> create_queue() {
  auto queue{std::make_shared<Queue>()};
  return queue;
}

void push(Queue *q, Task t) {
  std::lock_guard<std::mutex> lk(q->mut);
  q->queue.push(t);
  q->cond.notify_one();
}

std::shared_ptr<Task> wait_and_pop(Queue *q) {
  std::unique_lock<std::mutex> lk(q->mut);
  q->cond.wait(lk, [q] { return !q->queue.empty(); });
  auto res{std::make_shared<Task>(q->queue.front())};
  q->queue.pop();
  return res;
}

/*************************/
/*** PRIVATE FUNCTIONS ***/
/*************************/
