#include "Executors.h"

const std::list<std::string> Executors::get_running_tasks(std::string executor) {
    std::list<std::string> running_tasks = {};

    for(std::map<std::string, std::string>::iterator it = this->task_executor_map.begin(); 
            it != this->task_executor_map.end(); 
            ++it) {
        running_tasks.push_back(it->first);
    }

    return running_tasks;
}

void Executors::run_task(std::string task, std::string executor) {
    this->task_executor_map.insert(std::make_pair(task, executor));
}

void Executors::complete_task(std::string task) {
    this->task_executor_map.erase(task);
}

void Executors::add_executor(std::string host, std::string executor_name) {
    Executor exec = Executor(host, executor_name);
    this->executors.insert(std::make_pair(executor_name, exec)); 
}

void Executors::remove_executor(std::string executor_name) {
    this->executors.erase(executor_name);
}

