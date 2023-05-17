#ifndef TASK_H
#define TASK_H

#include <wrench-dev.h>
#include <string>
#include <list>

class Task {
    public:
        Task() {}
        Task(const std::string &name) : name(name) {}
        Task(const std::string &name, std::list<std::string> dependencies, float runtime) : 
            name(name), start_time(0), runtime(runtime), dependencies(dependencies) {}

        bool operator==(const Task &t) {
            return this->name.compare(t.name) == 0;
        }

        const std::list<std::string> &get_childs();
        
        void add_child(std::string task_name);
        void add_dependency(std::string task_name);
        void remove_dependency(std::string task_name);
        float get_expected_runtime();
        int get_number_dependencies();        
        float get_start_time();
        void set_start_time(float time);

    private:
        const std::string name;
        float start_time;
        float runtime;
        std::list<std::string> dependencies;
        std::list<std::string> childs;
};

#endif
