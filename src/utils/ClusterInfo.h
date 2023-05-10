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

        void set_number_unscheduled_tasks(int num);
        int get_number_unscheduled_tasks();

        void set_current_task_list(std::vector<std::string> tasks);
        bool is_priority(std::string task);
        bool is_last(std::string task);
        int get_number_remaining_tasks();
        std::list<wrench::WorkflowTask *> get_tasks(std::string vm);

        void run_task(std::string task, std::string executor);
        void complete_task(std::string task);
        const std::list<std::string> get_running_tasks(std::string executor);
        float predict_time(std::string task);
        void create_executor(std::string host, std::string executor);
        void remove_executor(std::string executor);

    private:
        std::unique_ptr<DAG> &dag;
        std::unique_ptr<Executors> &executors;        
};

#endif
