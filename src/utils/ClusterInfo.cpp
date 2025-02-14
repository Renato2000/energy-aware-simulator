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
    
    std::string host_name = this->executors->get_host_name(executor);
    this->hosts->use_core(host_name);
}

void ClusterInfo::complete_task(std::string task) {
    //this->dag->complete_task(task);

    const std::string &executor = this->executors->get_executor_task(task);
    std::string host_name = this->executors->get_host_name(executor);
    this->hosts->free_core(host_name);

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

float ClusterInfo::get_blevel(std::string task) {
    return this->dag->get_blevel(task);
}

int ClusterInfo::get_available_cores(std::string host) {
    return this->hosts->get_available_cores(host);
}

void ClusterInfo::add_executor(std::string host, std::string executor) {
    this->executors->add_executor(host, executor);
}

void ClusterInfo::add_energy(float energy) {
    this->total_energy += energy;   
}

float ClusterInfo::get_total_energy() {
    return this->total_energy;
}

float ClusterInfo::get_task_score(std::string task) {
    return this->dag->get_task_score(task);
}
