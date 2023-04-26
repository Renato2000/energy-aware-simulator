#include "Task.h"

void Task::add_child(Task task) {
    this->childs.push_back(task);
}

void Task::add_dependency(Task task) {
    this->dependencies.push_back(task);
}

void remove_dependency(Task task) {
    this->dependencies.remove(task);
}

std::list<Task> const &Task::get_childs() const {
    return this->childs;
}