#include "DAG.h"

DAG::DAG(std::string file_name) {
    
}

void DAG::concludeTask(std::string task_name) {
    if(this->tasks.count(task_name) == 0) return;

    std::unique_ptr<Task> &task = this->tasks[task_name];

    for(auto &t : task.get()->get_childs()) {
        if(this->tasks.count(task_name) != 0) {
            std::unique_ptr<Task> &child = this->tasks[t];
            child.get()->remove_dependency(task_name);
        }
    }
}
