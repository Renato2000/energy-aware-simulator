#ifndef DAG_H
#define DAG_H

#include "Task.h"

#include <memory>
#include <string>
#include <map>

class DAG {
    public:
        DAG(std::string file_name);
       
        void run_task(std::string task); 
        void complete_task(std::string task_name); 
        float get_expected_runtime(std::string task_name);
    private:
        std::map<std::string, std::unique_ptr<Task>> tasks;
};

#endif
