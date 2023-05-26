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

bool Executor::get_turn_off() {
    return this->turn_off;
}

void Executor::set_turn_off(bool value) {
    this->turn_off = value;
}

const std::string &Executor::get_host_name() {
    return this->host;
}