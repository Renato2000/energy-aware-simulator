#include "ClusterInfo.h"

void ClusterInfo::run_task(std::string task, std::string executor) {
    this->dag->complete_task(task);
    this->executors->run_task(task, executor);
}

void ClusterInfo::complete_task(std::string task) {
    //this->dag->complete_task(task);
    this->executors->complete_task(task);
}

const std::list<std::string> ClusterInfo::get_running_tasks(std::string executor) {
    return this->executors->get_running_tasks(executor);
}

float ClusterInfo::predict_time(std::string task) {
    return this->dag->get_expected_runtime(task);
}

void ClusterInfo::create_executor(std::string host, std::string executor) {
    this->executors->add_executor(host, executor);
}

void ClusterInfo::remove_executor(std::string executor) {
    this->executors->remove_executor(executor);
}

void ClusterInfo::set_current_task_list(std::vector<std::string> tasks) {
    
}

bool ClusterInfo::is_priority(std::string task) {
    return false;
}

bool ClusterInfo::is_last(std::string task) {
    return false;
}

int ClusterInfo::get_number_remaining_tasks() {
    return 0;
}

std::list<wrench::WorkflowTask *> ClusterInfo::get_tasks(std::string vm) {
    std::list<wrench::WorkflowTask *> ret;

    return ret;
}

