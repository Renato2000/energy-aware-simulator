#include "ClusterInfo.h"

void ClusterInfo::set_current_task_list(std::vector<wrench::WorkflowTask *> tasks) {
    this->current_task_list = tasks;
}

int ClusterInfo::get_number_remaining_tasks() {
    return this->current_task_list.size();
}

bool ClusterInfo::is_priority(std::string task) {
    return false;
}

bool ClusterInfo::is_last(std::string task) {
    return false;
}

std::list<wrench::WorkflowTask *> ClusterInfo::get_tasks(std::string vm) {
    std::list<wrench::WorkflowTask *> ret;

    return ret;
}

float ClusterInfo::predict_time(std::string task) {
    return this->dag->get_expected_runtime(task);
}

void ClusterInfo::run_task(std::string task, std::string executor) {
    this->dag->complete_task(task);
    this->executors->run_task(task, executor);
}


void ClusterInfo::complete_task(std::string task) {
    //this->dag->complete_task(task);
    this->executors->complete_task(task);
}
