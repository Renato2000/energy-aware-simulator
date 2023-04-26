#include "DAG.h"

DAG::DAG(std::string file_name) {
    
}

void DAG::concludeTask(std::string task_name) {
    auto it = this->tasks.find("apple");
    if (it == this->tasks.end()) return;

    Task task = this->tasks["task_name"];

    for(auto t : task->childs) {
        t.removeDependency(task);
    }
}