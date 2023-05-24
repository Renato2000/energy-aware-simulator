#include "ClusterInfo.h"

void ClusterInfo::set_current_task_list(std::vector<wrench::WorkflowTask *> tasks) {
    this->remaining_tasks = tasks.size();
    this->current_task_list = tasks;
}

int ClusterInfo::get_number_remaining_tasks() {
    return this->remaining_tasks;
}

bool ClusterInfo::is_priority(std::string task) {
    return this->dag->is_priority(task);
}

bool ClusterInfo::is_last(std::string task_name) {
    wrench::WorkflowTask *task = this->current_task_list.back();
    return task->getID() == task_name;
}

std::list<std::string> ClusterInfo::get_tasks(std::string vm) {
    return this->executors->get_tasks(vm);
}

float ClusterInfo::predict_time(std::string task) {
    return this->dag->get_expected_runtime(task);
}

void ClusterInfo::run_task(std::string task, std::string executor) {
    this->remaining_tasks -= 1;
    this->dag->complete_task(task);
    this->executors->run_task(task, executor);
}

void ClusterInfo::complete_task(std::string task) {
    //this->dag->complete_task(task);
    this->executors->complete_task(task);
}

float ClusterInfo::get_start_time(std::string task) {
    return this->dag->get_start_time(task);
}

void ClusterInfo::set_start_time(std::string task, float time) {
    this->dag->set_start_time(task, time);
}

bool ClusterInfo::get_turn_off(std::string executor) {
    return this->executors->get_turn_off(executor);
}

void ClusterInfo::set_turn_off(std::string executor, bool value) {
    this->executors->set_turn_off(executor, value);
}

const std::vector<std::string> ClusterInfo::get_hosts() {
    return this->hosts->get_hosts();
}

int ClusterInfo::get_host_cores(std::string host) {
    return this->hosts->get_host_cores(host);
}

void ClusterInfo::add_algorithm_time(long time) {
    this->alg_time += time;
}

long ClusterInfo::get_algorithm_time() {
    return this->alg_time;
}
