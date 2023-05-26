#ifndef EXECUTORS_H
#define EXECUTORS_H

#include "Executor.h"

#include <map>
#include <string>
#include <list>

class Executors {
    public:
        Executors() {}
        
        void run_task(std::string task, std::string executor);
        void complete_task(std::string task);       
        void add_executor(std::string host, std::string executor);
        void remove_executor(std::string executor);        
        std::list<std::string> get_tasks(std::string executor);        
        void set_turn_off(std::string executor, bool value);
        bool get_turn_off(std::string executor);
        const std::string &get_host_name(std::string executor);
        const std::string &Executors::get_executor_task(std::string task_name);

    private:
        std::map<std::string, std::string> task_executor_map;
        std::map<std::string, Executor> executors;
};

#endif
