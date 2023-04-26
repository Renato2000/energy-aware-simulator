#ifndef TASK_H
#define TASK_H

#include <string>
#include <list>

class Task {
    public:
        Task(const std::string &name) : name(name) {}

        bool operator==(const Task &t) {
            return this->name.compare(t.name) == 0;
        }

        void add_child(Task task);
        void add_dependency(Task task);
        void remove_dependency(Task task);
        std::list<Task> const &get_childs() const

    private:
        std::string name;
        std::list<Task> dependencies;
        std::list<Task> childs;
};

#endif
