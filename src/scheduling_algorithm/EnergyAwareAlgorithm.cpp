#include "EnergyAwareAlgorithm.h"

#include <limits>

WRENCH_LOG_CATEGORY(enegy_aware_algorithm, "Log category for EnergyAwareAlgorithm");

/**
 *
 * @param cloud_service
 * @param power_model
 */
EnergyAwareAlgorithm::EnergyAwareAlgorithm(std::shared_ptr<ClusterInfo> cluster_info,
                                    std::shared_ptr<wrench::CloudComputeService> &cloud_service,
                                    std::unique_ptr<CostModel> cost_model)
        : SchedulingAlgorithm(cloud_service, std::move(cost_model)),
            cluster_info(cluster_info) {
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

    // get idle vms
    std::vector<std::string> idle_vms;
    for (const auto &vm : candidate_vms) {
        if (this->cloud_service->isVMRunning(vm) &&
            this->cloud_service->getVMComputeService(vm)->getTotalNumIdleCores() > 0) {
            idle_vms.push_back(vm);
        }
    }

    if (cluster_info->get_number_remaining_tasks() < getTotalNumberCores(candidate_vms)) {
        if (getTotalNumberIdleCores(candidate_vms) == 0) return "";
        if (cluster_info->is_priority(task->getID()) || cluster_info->is_last(task->getID())) return idle_vms[0];

        std::string best_fit = getBestFit(task->getID(), candidate_vms);
        if (isIdle(best_fit)) return best_fit;
        else return "";             
    }
    else if (idle_vms.size() > 0) {
        return idle_vms[0];
    }
    else {
        std::string vm_name;
    
        // find idle vm
        for (const auto &vm : candidate_vms) {
            if (this->cloud_service->isVMDown(vm)) {
                vm_name = vm;
            }
        }

        if (vm_name.empty()) {            
            if (getNumberIdleHosts() == 0) return "";
        
            // find idle host
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
            // turn on idle host
            if (turned_on) {
                vm_name = this->cloud_service->createVM(num_cores, 1000000000);
            }
        }

        if (vm_name.empty()) return "";
        
        // start new VM
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

int EnergyAwareAlgorithm::getTotalNumberCores(std::vector<std::string> candidate_vms) {
    int num_cores = 0;

    for (const auto &vm : candidate_vms) {
        if (this->cloud_service->isVMRunning(vm)) {
            num_cores += this->cloud_service->getVMComputeService(vm)->getTotalNumCores();
        }
    }
    
    return num_cores;
}

int EnergyAwareAlgorithm::getTotalNumberIdleCores(std::vector<std::string> candidate_vms) {
    int num_cores = 0;
    
    for (const auto &vm : candidate_vms) {
        if (this->cloud_service->isVMRunning(vm)) {
            num_cores += this->cloud_service->getVMComputeService(vm)->getTotalNumIdleCores();
        }
    }

    return num_cores;
}

std::string EnergyAwareAlgorithm::getBestFit(std::string task, std::vector<std::string> candidate_vms) {
    float best_time = std::numeric_limits<float>::max();
    std::string best_exec = "";

    for (const auto &vm : candidate_vms) {
        float highest = std::numeric_limits<float>::min();
        float lowest = std::numeric_limits<float>::max();

        std::list<wrench::WorkflowTask *> tasks_vm = cluster_info->get_tasks(vm);
        
        if (this->cloud_service->getVMComputeService(vm)->getTotalNumIdleCores() > 0) { 
            float t_low = wrench::Simulation::getCurrentSimulatedDate();
        }
        else {
            for (auto *t : tasks_vm) {
                if(t->getStartDate() + cluster_info->predict_time(t->getID()) < lowest)
                    lowest = t->getStartDate() + cluster_info->predict_time(t->getID());
            }

        }
        
        for (auto *t : tasks_vm) {
            if(t->getStartDate() + cluster_info->predict_time(t->getID()) > highest)
                highest = t->getStartDate() + cluster_info->predict_time(t->getID());
        }

        float idle = highest - lowest;
        float distance = abs(cluster_info->predict_time(task) - idle);
        if (distance < best_time) {
            best_time = distance;
            best_exec = vm; 
        }
    }

    return best_exec;
}

int EnergyAwareAlgorithm::getNumberIdleHosts() {
    int num_hosts = 0;
    
    for (auto &host : this->cloud_service->getExecutionHosts()) {
        if (!wrench::Simulation::isHostOn(host)) num_hosts++;
    }

    return num_hosts;
}

std::string EnergyAwareAlgorithm::getIdleHost() {
    for (auto &host : this->cloud_service->getExecutionHosts()) {
        if (!wrench::Simulation::isHostOn(host)) return host;
    }

    return "";
}

bool EnergyAwareAlgorithm::isIdle(std::string vm) {
    return false;
}
