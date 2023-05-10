#ifndef CLUSTER_INFO_H
#define CLUSTER_INFO_H

#include "DAG.h"
#include "Executors.h"

#include <wrench-dev.h>
#include <memory>
#include <vector>

class ClusterInfo {
    public:
        ClusterInfo(std::unique_ptr<DAG> &dag, std::unique_ptr<Executors> &executors) :
            dag(dag),
            executors(executors) {}

        void set_current_task_list(std::vector<wrench::WorkflowTask *> tasks);
        int get_number_remaining_tasks();
        bool is_priority(std::string task);
        bool is_last(std::string task);
        std::vector<wrench::WorkflowTask *> get_tasks(std::string vm);
        float predict_time(std::string task);
        void run_task(std::string task, std::string executor);
        void complete_task(std::string task);

    private:
        std::vector<wrench::WorkflowTask *> current_task_list;
        std::unique_ptr<DAG> &dag;
        std::unique_ptr<Executors> &executors;        
};

#endif
