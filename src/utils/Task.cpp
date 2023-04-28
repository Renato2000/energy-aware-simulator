#include "Task.h"

void Task::add_child(std::string task_name) {
    this->childs.push_back(task_name);
}

void Task::add_dependency(std::string task_name) {
    this->dependencies.push_back(task_name);
}

void Task::remove_dependency(std::string task_name) {
    this->dependencies.remove(task_name);
}

const std::list<std::string> &Task::get_childs() {
    return this->childs;
}
