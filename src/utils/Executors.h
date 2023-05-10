#ifndef EXECUTORS_H
#define EXECUTORS_H

#include "Executor.h"

#include <map>
#include <string>
#include <list>

class Executors {
    public:
        Executors() {}
        
        const std::list<std::string> get_running_tasks(std::string);
        
        void run_task(std::string task, std::string executor);
        void complete_task(std::string task);       
        void add_executor(std::string host, std::string executor);
        void remove_executor(std::string executor);        

    private:
        std::map<std::string, std::string> task_executor_map;
        std::map<std::string, Executor> executors;
};

#endif
