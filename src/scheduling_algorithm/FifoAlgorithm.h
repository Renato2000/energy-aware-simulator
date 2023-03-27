#ifndef ENERGY_AWARE_FIFOALGORITHM_H
#define ENERGY_AWARE_FIFOALGORITHM_H

#include "SchedulingAlgorithm.h"

class FifoAlgorithm : public SchedulingAlgorithm {
public:
    FifoAlgorithm(std::shared_ptr<wrench::CloudComputeService> &cloud_service,
                     std::unique_ptr<CostModel> cost_model);

    std::vector<wrench::WorkflowTask *> sortTasks(const std::vector<wrench::WorkflowTask *> &tasks) override;

    std::string scheduleTask(const wrench::WorkflowTask *task) override;

    void notifyVMShutdown(const std::string &vm_name, const std::string &vm_pm) override;

protected:
    std::map<const wrench::WorkflowTask *, std::string> task_to_host_schedule;
};


#endif //ENERGY_AWARE_FIFOALGORITHM_H
