#ifndef DAG_H
#define DAG_H

#include "Task.h"

#include <wrench-dev.h>
#include <memory>
#include <string>
#include <map>

class DAG {
    public:
        DAG(std::string file_name);
       
        void run_task(std::string task); 
        void complete_task(std::string task_name); 
        float get_expected_runtime(std::string task_name);
        bool is_priority(std::string task_name);
        void set_start_time(std::string task_name, float start_time);
        float get_start_time(std::string task_name);
 
    private:
        std::map<std::string, std::unique_ptr<Task>> tasks;
};

#endif
