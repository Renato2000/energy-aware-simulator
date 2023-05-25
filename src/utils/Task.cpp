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

int Task::get_number_dependencies() {
    return this->dependencies.size();
}

float Task::get_start_time() {
    return this->start_time;
}

void Task::set_start_time(float start_time) {
    this->start_time = start_time;
}

float Task::get_blevel() {
    return this->blevel;
}

void Task::set_blevel(float value) {
    this->blevel = value;
}
