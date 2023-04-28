#include "Task.h"

const std::list<std::string> &Task::get_childs() {
    return this->childs;
}

void Task::add_child(std::string task_name) {
    this->childs.push_back(task_name);
}

void Task::add_dependency(std::string task_name) {
    this->dependencies.push_back(task_name);
}

void Task::remove_dependency(std::string task_name) {
    this->dependencies.remove(task_name);
}

float Task::get_expected_runtime() {
    return this->runtime;
}
