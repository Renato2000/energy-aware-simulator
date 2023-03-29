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

    std::cout << "Scheduling a new task" << std::endl;

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
            std::cout << vm << " is running and has available idle cores" << std::endl;
            return vm;

        } else if (vm_name.empty() && this->cloud_service->isVMDown(vm)) {
            std::cout << vm << " is down but could be turned on to schedule tasks..." << std::endl;
            vm_name = vm;
        }
    }

    if (vm_name.empty()) {
        std::cout << "Could not find a suitable vm" << std::endl;
        // create VM, as no viable VM could be found
        if (this->cloud_service->getTotalNumIdleCores() == 0) {
            std::cout << "There are not enough resources on the cluster" << std::endl;
            return "";
        }
        bool turned_on = false;
        int num_cores = 1;
        for (auto &host : this->cloud_service->getExecutionHosts()) {
            if (!wrench::Simulation::isHostOn(host)) {
                std::cout << host << " is turned off, so it will be turned on" << std::endl;
                wrench::Simulation::turnOnHost(host);
                num_cores = wrench::Simulation::getHostNumCores(host);
                turned_on = true;
                break;
            }
        }
        if (turned_on) {
            std::cout << "Creating a new VM" << std::endl;
            vm_name = this->cloud_service->createVM(num_cores, 1000000000);
        }
    }

    if (vm_name.empty()) {
        std::cout << "Could not find a VM" << std::endl;
        return "";
    }

    // start VM
    auto vm_cs = cloud_service->getVMComputeService(vm_name);
    std::cout << "Starting VM " << vm_name << " which has " << vm_cs->getTotalNumIdleCores() << " idle cores" << std::endl;
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
