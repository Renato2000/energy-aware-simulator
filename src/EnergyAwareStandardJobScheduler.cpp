/**
 * Copyright (c) 2020-2021. The WRENCH Team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "EnergyAwareStandardJobScheduler.h"

#include <utility>
#include <chrono>

WRENCH_LOG_CATEGORY(energy_aware_scheduler, "Log category for EnergyAwareStandardJobScheduler");

/**
 * @brief Constructor, which calls the super constructor
 *
 * @param storage_service: default storage service available for the scheduler
 */
EnergyAwareStandardJobScheduler::EnergyAwareStandardJobScheduler(
        std::shared_ptr<wrench::StorageService> storage_service,
        std::unique_ptr<SchedulingAlgorithm> scheduling_algorithm,
        std::shared_ptr<ClusterInfo> cluster_info) :
        default_storage_service(std::move(storage_service)),
        scheduling_algorithm(std::move(scheduling_algorithm)), 
        cluster_info(cluster_info) {

    this->unscheduled_tasks = 0;
}

/**
 * @brief A method that schedules tasks (as part of standard jobs), according to whatever decision algorithm
 *        it implements, over a set of compute services
 * @param compute_services: the set of compute services
 * @param tasks: the set of tasks to be executed
 */
void EnergyAwareStandardJobScheduler::scheduleTasks(
        const std::set<std::shared_ptr<wrench::ComputeService>> &compute_services,
        const std::vector<wrench::WorkflowTask *> &tasks) {

    auto start = std::chrono::high_resolution_clock::now();

    // If nothing to do, return;
    if (compute_services.empty() or tasks.empty()) {
        return;
    } else if (compute_services.size() > 1) {
        throw std::runtime_error("This Energy-Aware Cloud Scheduler can only handle a single compute service");
    }

    WRENCH_INFO("There are %ld ready tasks to schedule", tasks.size());
    this->unscheduled_tasks = tasks.size();

    // Sort tasks
    auto sorted_tasks = this->scheduling_algorithm->sortTasks(tasks);

    this->cluster_info->set_current_task_list(sorted_tasks);

    // obtaining cloud service
    auto cloud_service = std::dynamic_pointer_cast<wrench::CloudComputeService>(*compute_services.begin());
    if (cloud_service == nullptr) {
        throw std::runtime_error("This Energy-Aware Cloud Scheduler can only handle a cloud service");
    }

    //std::cout << "==================New list of tasks to schedule ==================" << std::endl;

    // attempting to schedule tasks
    for (auto const &task : sorted_tasks) {
        auto vm_name = this->scheduling_algorithm->scheduleTask(task);

        //std::cout << "Choosen vm: " << vm_name << std::endl;

        if (!vm_name.empty()) {
            
            //std::cout << "[Scheduler] Start task " << task->getID() << " on vm " << vm_name << " at " << wrench::Simulation::getCurrentSimulatedDate() << std::endl;

            // finding the file locations
            std::map<wrench::WorkflowFile *, std::shared_ptr<wrench::FileLocation>> file_locations;
            for (auto f : task->getInputFiles()) {
                file_locations[f] = wrench::FileLocation::LOCATION(this->default_storage_service);
            }
            for (auto f : task->getOutputFiles()) {
                file_locations[f] = wrench::FileLocation::LOCATION(this->default_storage_service);
            }

            // creating job for execution
            std::shared_ptr<wrench::WorkflowJob> job =
                    (std::shared_ptr<wrench::WorkflowJob>) this->getJobManager()->createStandardJob(task,
                                                                                                    file_locations);

            WRENCH_INFO("Scheduling task: %s", task->getID().c_str());
            auto vm_cs = cloud_service->getVMComputeService(vm_name);
            this->getJobManager()->submitJob(job, vm_cs);
            this->tasks_vm_map.insert(std::pair<wrench::WorkflowTask *, std::string>(task, vm_name));
            this->unscheduled_tasks--;
            
            this->cluster_info->run_task(task->getID(), vm_name); 
            this->cluster_info->set_start_time(task->getID(), wrench::Simulation::getCurrentSimulatedDate());
       }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    this->cluster_info->add_algorithm_time(duration);
}

/**
 * Notify that a task has completed its execution.
 * @param compute_services
 * @param task Pointer to task that has completed its execution.
 */
void EnergyAwareStandardJobScheduler::notifyTaskCompletion(
        const std::set<std::shared_ptr<wrench::ComputeService>> &compute_services,
        wrench::WorkflowTask *task) {
    
        
    // std::cout << "[Scheduler] Task " << task->getID() << " ended at " << wrench::Simulation::getCurrentSimulatedDate() << std::endl;
    this->cluster_info->complete_task(task->getID());
    if (this->unscheduled_tasks > 0) {
        this->unscheduled_tasks--;
    } 

    auto it = this->tasks_vm_map.find(task);
    auto cloud_service = std::dynamic_pointer_cast<wrench::CloudComputeService>(*compute_services.begin());
    auto vm_cs = cloud_service->getVMComputeService(it->second);
    try {
        if (vm_cs->getTotalNumCores() == vm_cs->getTotalNumIdleCores()) {
		    auto vm_pm = cloud_service->getVMPhysicalHostname(it->second);
			// std::cout << "[Scheduler] Turn off vm " << vm_pm << " at " << wrench::Simulation::getCurrentSimulatedDate() << std::endl;
            //cloud_service->shutdownVM(it->second);
            //this->scheduling_algorithm->notifyVMShutdown(it->second, vm_pm);
	    }
	}
	catch (wrench::WorkflowExecutionException &e) {
        // ignore
    }
}
