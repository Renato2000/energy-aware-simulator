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
 
/* 
    std::sort(sorted_tasks.begin(), sorted_tasks.end(),
            [this](const wrench::WorkflowTask *t1, const wrench::WorkflowTask *t2) -> bool {
                if (cluster_info->predict_time(t1->getID()) == cluster_info->predict_time(t2->getID())) {
                    return ((uintptr_t) t1 < (uintptr_t) t2);
                } else {
                    return (cluster_info->predict_time(t1->getID()) > cluster_info->predict_time(t2->getID()));
                }
            }); 

*/
/*
    std::sort(sorted_tasks.begin(), sorted_tasks.end(),
            [this](const wrench::WorkflowTask *t1, const wrench::WorkflowTask *t2) -> bool {
                if (cluster_info->is_priority(t1->getID()) && !cluster_info->is_priority(t2->getID())) {
                    return true;
                } else if (cluster_info->is_priority(t2->getID()) && !cluster_info->is_priority(t1->getID())) {
                    return false;
                } else if (cluster_info->predict_time(t1->getID()) == cluster_info->predict_time(t2->getID())) {
                    return ((uintptr_t) t1 < (uintptr_t) t2);
                } else {
                    return (cluster_info->predict_time(t1->getID()) > cluster_info->predict_time(t2->getID()));
                }
            });
*/

     std::sort(sorted_tasks.begin(), sorted_tasks.end(),
            [this](const wrench::WorkflowTask *t1, const wrench::WorkflowTask *t2) -> bool {
                if (cluster_info->get_blevel(t1->getID()) == cluster_info->get_blevel(t2->getID())) {
                    return ((uintptr_t) t1 < (uintptr_t) t2);
                } else {
                    return (cluster_info->get_blevel(t1->getID()) > cluster_info->get_blevel(t2->getID()));
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
    
    /*
    std::cout << "Analyzing task: " << task->getID() << "----------------------------------------------" << std::endl;
    std::cout << "Available executors: " << std::endl;
    for(auto &it : vm_worker_map) {
        auto vm = it.first;
        std::cout << "\tvm: " << vm << " with " << this->cloud_service->getVMComputeService(vm)->getTotalNumIdleCores() << " idle cores" << std::endl;
    }
    */

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

    if (idle_vms.size() > 0 && cluster_info->get_number_remaining_tasks() <= getTotalNumberCores(candidate_vms)) { 
        if (getTotalNumberIdleCores(candidate_vms) == 0) {
            //std::cout << "no cores available" << std::endl;
            return "";
        }
        if (cluster_info->is_priority(task->getID())) {
            //std::cout << "is priority" << std::endl;
            return idle_vms[0];
        }

        std::string best_fit = getBestFit(task->getID(), candidate_vms);
        if (isIdle(best_fit)) {
            //std::cout << "best_fit is idle" << std::endl;
            return best_fit;
        }
        else {
            //std::cout << "best_fit not idle" << std::endl;
            for (const auto &vm : idle_vms) {
                if (cluster_info->predict_time(task->getID()) < calculateIdleTime(vm)) return vm;
            }
            return "";
        }
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

        // find idle host
        if (vm_name.empty()) {            
            if (getNumberIdleHosts() == 0) return "";
        
            bool turned_on = false;
            int num_cores = 1;
            for (auto &host : this->cloud_service->getExecutionHosts()) {
                if (!wrench::Simulation::isHostOn(host) && 
                  (cluster_info->get_number_remaining_tasks() >= getTotalNumberCores(candidate_vms) + cluster_info->get_host_cores(host)
                  || candidate_vms.size() == 0)) {
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

float EnergyAwareAlgorithm::calculateIdleTime(std::string vm) {
    float highest = std::numeric_limits<float>::min();
    float lowest = std::numeric_limits<float>::max();

    std::list<std::string> tasks_vm = cluster_info->get_tasks(vm);

    if (this->cloud_service->getVMComputeService(vm)->getTotalNumIdleCores() > 0) { 
        lowest = wrench::Simulation::getCurrentSimulatedDate();
    }
    else {
        for (auto t_name : tasks_vm) {
            float finish = cluster_info->get_start_time(t_name) + cluster_info->predict_time(t_name);
            if(finish < lowest) lowest = finish;
        }

    }
        
    if (tasks_vm.size() == 0) highest = wrench::Simulation::getCurrentSimulatedDate();
    else {
        for (auto t_name : tasks_vm) {
            if(cluster_info->get_start_time(t_name) + cluster_info->predict_time(t_name) > highest)
                highest = cluster_info->get_start_time(t_name) + cluster_info->predict_time(t_name);
        }
    }

    return abs(highest - lowest);
}

std::string EnergyAwareAlgorithm::getBestFit(std::string task, std::vector<std::string> candidate_vms) {
    float best_time = std::numeric_limits<float>::max();
    std::string best_exec = "";

    for (const auto &vm : candidate_vms) {

        float idle = this->calculateIdleTime(vm); 
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
    auto ret = this->cloud_service->getVMComputeService(vm)->getTotalNumIdleCores() > 0;
    return ret;
}
