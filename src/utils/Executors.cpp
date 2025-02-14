#include "Executors.h"

#include <iostream>

void Executors::run_task(std::string task, std::string executor) {
    this->executors[executor].place_task(task);
    this->task_executor_map.insert(std::make_pair(task, executor));
}

void Executors::complete_task(std::string task) {
    std::string executor = this->task_executor_map[task];
    this->executors[executor].conclude_task(task); 
    this->task_executor_map.erase(task);
}

void Executors::add_executor(std::string host, std::string executor_name) {
    Executor exec = Executor(host, executor_name);
    this->executors.insert(std::make_pair(executor_name, exec)); 
}

void Executors::remove_executor(std::string executor_name) {
    this->executors.erase(executor_name);
}

std::list<std::string> Executors::get_tasks(std::string executor_name) {
    return this->executors[executor_name].get_running_tasks();
}

void Executors::set_turn_off(std::string executor, bool value) {
    this->executors[executor].set_turn_off(value);
}

bool Executors::get_turn_off(std::string executor) {
    return this->executors[executor].get_turn_off();
}

const std::string &Executors::get_host_name(std::string executor) {
    return this->executors[executor].get_host_name();
}

const std::string &Executors::get_executor_task(std::string task_name) {
    return this->task_executor_map[task_name];
} 
