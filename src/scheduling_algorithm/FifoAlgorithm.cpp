#include "FifoAlgorithm.h"

WRENCH_LOG_CATEGORY(fifo_algorithm, "Log category for FifoAlgorithm");

/**
 *
 * @param cloud_service
 * @param power_model
 */
FifoAlgorithm::FifoAlgorithm(std::shared_ptr<wrench::CloudComputeService> &cloud_service,
                                   std::unique_ptr<CostModel> cost_model)
        : SchedulingAlgorithm(cloud_service, std::move(cost_model)) {
}

/**
 *
 * @param tasks
 * @return
 */
std::vector<wrench::WorkflowTask *> FifoAlgorithm::sortTasks(const vector<wrench::WorkflowTask *> &tasks) {
    return tasks;
}

/**
 *
 * @param task
 * @return
 */
std::string FifoAlgorithm::scheduleTask(const wrench::WorkflowTask *task) {

    if (this->task_to_host_schedule.find(task) == this->task_to_host_schedule.end()) {
        return "";
    }

    auto host = this->task_to_host_schedule.at(task);
    if (!wrench::Simulation::isHostOn(host)) {
        wrench::Simulation::turnOnHost(host);
    }

    // find candidate vms
    std::vector<std::string> candidate_vms;
    for (auto &it : this->vm_worker_map) {
        if (it.second == host) {
            candidate_vms.push_back(it.first);
        }
    }

    // look for a running, idle VM
    std::string vm_name;
    for (const auto &vm : candidate_vms) {
        if (this->cloud_service->isVMRunning(vm) &&
            this->cloud_service->getVMComputeService(vm)->getTotalNumIdleCores() > 0) {
            return vm;

        } else if (vm_name.empty() && this->cloud_service->isVMDown(vm)) {
            vm_name = vm;
        }
    }

    if (vm_name.empty()) {
        // create VM, as no viable VM could be found
        if (this->cloud_service->getPerHostNumIdleCores().at(host) == 0) {
            return "";
        }
        vm_name = this->cloud_service->createVM(1, 1000000000);
    }

    // start VM
    this->cloud_service->startVM(vm_name);
    auto vm_pm = this->cloud_service->getVMPhysicalHostname(vm_name);

    if (this->vm_worker_map.find(vm_name) == this->vm_worker_map.end()) {
        this->vm_worker_map.insert(std::pair<std::string, std::string>(vm_name, vm_pm));

        if (this->worker_running_vms.find(vm_pm) == this->worker_running_vms.end()) {
            this->worker_running_vms.insert(std::pair<std::string, int>(vm_pm, 0));
        }
    }
    this->worker_running_vms.at(vm_pm)++;

    return vm_name;
}

/**
 *
 * @param vm_name
 * @param vm_pm
 */
void FifoAlgorithm::notifyVMShutdown(const string &vm_name, const string &vm_pm) {
    this->worker_running_vms.at(vm_pm)--;
    if (this->worker_running_vms.at(vm_pm) == 0) {
        wrench::Simulation::turnOffHost(vm_pm);
    }
}
