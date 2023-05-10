#include "Executor.h"

const std::list<std::string> &Executor::get_running_tasks() {
    return this->tasks;
}

void Executor::place_task(std::string task) {
   this->tasks.push_back(task); 
}

void Executor::conclude_task(std::string task) {
    this->tasks.remove(task);
}
