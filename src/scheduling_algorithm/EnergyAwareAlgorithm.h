#ifndef ENERGY_AWARE_ALGORITHM_H
#define ENERGY_AWARE_ALGORITHM_H

#include "SchedulingAlgorithm.h"
#include "cost_model/CostModel.h"
#include "utils/DAG.h"

class EnergyAwareAlgorithm : public SchedulingAlgorithm {
public:
    EnergyAwareAlgorithm(std::unique_ptr<DAG> &dag,
                    std::shared_ptr<wrench::CloudComputeService> &cloud_service,
                    std::unique_ptr<CostModel> cost_model);

    std::vector<wrench::WorkflowTask *> sortTasks(const std::vector<wrench::WorkflowTask *> &tasks) override;

    std::string scheduleTask(const wrench::WorkflowTask *task) override;

    void notifyVMShutdown(const std::string &vm_name, const std::string &vm_pm) override;

private:
    std::unique_ptr<DAG> dag;
    std::set<std::string> vms_pool;
};

#endif
