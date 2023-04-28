#include "EnergyAwareAlgorithm.h"

WRENCH_LOG_CATEGORY(enegy_aware_algorithm, "Log category for EnergyAwareAlgorithm");

/**
 *
 * @param cloud_service
 * @param power_model
 */
EnergyAwareAlgorithm::EnergyAwareAlgorithm(std::unique_ptr<DAG> &dag,
                                    std::shared_ptr<wrench::CloudComputeService> &cloud_service,
                                    std::unique_ptr<CostModel> cost_model)
        : SchedulingAlgorithm(cloud_service, std::move(cost_model)),
            dag(std::move(dag)) {
}

/**
 *
 * @param tasks
 * @return
 */
std::vector<wrench::WorkflowTask *> EnergyAwareAlgorithm::sortTasks(const vector<wrench::WorkflowTask *> &tasks) {
    auto sorted_tasks = tasks;

    std::sort(sorted_tasks.begin(), sorted_tasks.end(),
              [](const wrench::WorkflowTask *t1, const wrench::WorkflowTask *t2) -> bool {
                  if (t1->getAverageCPU() == t2->getAverageCPU()) {
                      return ((uintptr_t) t1 < (uintptr_t) t2);
                  } else {
                      return (t1->getAverageCPU() > t2->getAverageCPU());
                  }
              });

    return sorted_tasks;
}

/**
 *
 * @param task
 * @return
 */
std::string EnergyAwareAlgorithm::scheduleTask(const wrench::WorkflowTask *task) {

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
void EnergyAwareAlgorithm::notifyVMShutdown(const string &vm_name, const string &vm_pm) {
    this->vm_worker_map.erase(vm_name);
    this->worker_running_vms.at(vm_pm)--;
    if (this->worker_running_vms.at(vm_pm) == 0) {
        wrench::Simulation::turnOffHost(vm_pm);
    }
}
