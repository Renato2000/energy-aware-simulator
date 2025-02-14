#include "FifoAlgorithm.h"

WRENCH_LOG_CATEGORY(fifo_algorithm, "Log category for FifoAlgorithm");

/**
 *
 * @param cloud_service
 * @param power_model
 */
FifoAlgorithm::FifoAlgorithm(std::shared_ptr<ClusterInfo> cluster_info,
                                std::shared_ptr<wrench::CloudComputeService> &cloud_service,
                                std::unique_ptr<CostModel> cost_model)
        : SchedulingAlgorithm(cloud_service, std::move(cost_model)), cluster_info(cluster_info) {
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

    // find candidate vms
    std::vector<std::string> candidate_vms;
    for (auto &it : this->vm_worker_map) {
        candidate_vms.push_back(it.first);
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
        if (this->cloud_service->getTotalNumIdleCores() == 0) {
            return "";
        }
        bool turned_on = false;
        int num_cores = 1;
        for (auto &host : this->cloud_service->getExecutionHosts()) {
            if (!wrench::Simulation::isHostOn(host)) {
                wrench::Simulation::turnOnHost(host);
                num_cores = wrench::Simulation::getHostNumCores(host);
                turned_on = true;
                break;
            }
        }
        if (turned_on) {
            vm_name = this->cloud_service->createVM(num_cores, 1000000000);
        }
    }

    if (vm_name.empty()) {
        return "";
    }

    // start VM
    this->cloud_service->startVM(vm_name);
    auto vm_pm = this->cloud_service->getVMPhysicalHostname(vm_name);

    this->cluster_info->add_executor(vm_pm, vm_name);

    //std::cout << "[Scheduler] Start vm " << vm_name << " on host " << vm_pm << " with " << wrench::Simulation::getHostNumCores(vm_pm) << " cores at " << wrench::Simulation::getCurrentSimulatedDate() << std::endl;

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
    this->vm_worker_map.erase(vm_name);
    this->worker_running_vms.at(vm_pm)--;
    if (this->worker_running_vms.at(vm_pm) == 0) {
        wrench::Simulation::turnOffHost(vm_pm);
    }
}
