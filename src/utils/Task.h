#ifndef TASK_H
#define TASK_H

#include <string>
#include <list>

class Task {
    public:
        Task() {}
        Task(const std::string &name) : name(name) {}
        Task(const std::string &name, std::list<std::string> dependencies, float runtime) : 
            name(name), runtime(runtime), dependencies(dependencies) {}

        bool operator==(const Task &t) {
            return this->name.compare(t.name) == 0;
        }

        const std::list<std::string> &get_childs();
        
        void add_child(std::string task_name);
        void add_dependency(std::string task_name);
        void remove_dependency(std::string task_name);
        float get_expected_runtime();

    private:
        const std::string name;
        float runtime;
        std::list<std::string> dependencies;
        std::list<std::string> childs;
};

#endif
