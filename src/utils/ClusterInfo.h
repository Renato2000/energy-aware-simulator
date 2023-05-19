#ifndef CLUSTER_INFO_H
#define CLUSTER_INFO_H

#include "DAG.h"
#include "Executors.h"
#include "Hosts.h"

#include <wrench-dev.h>
#include <memory>
#include <vector>

class ClusterInfo {
    public:
        ClusterInfo(std::unique_ptr<DAG> &dag, std::unique_ptr<Executors> &executors, std::unique_ptr<Hosts> &hosts) :
            dag(dag),
            executors(executors),
            hosts(hosts) {}

        void set_current_task_list(std::vector<wrench::WorkflowTask *> tasks);
        int get_number_remaining_tasks();
        bool is_priority(std::string task);
        bool is_last(std::string task);
        std::list<std::string> get_tasks(std::string vm);
        float predict_time(std::string task);
        void run_task(std::string task, std::string executor);
        void complete_task(std::string task);
        float get_start_time(std::string task);
        void set_start_time(std::string task, float time);
        bool get_turn_off(std::string executor);
        void set_turn_off(std::string executor, bool value);
        const std::vector<std::string> get_hosts();
        int get_host_cores(std::string host);

    private:
        int remaining_tasks;
        std::vector<wrench::WorkflowTask *> current_task_list;
        std::unique_ptr<DAG> &dag;
        std::unique_ptr<Executors> &executors;
        std::unique_ptr<Hosts> &hosts;
};

#endif
