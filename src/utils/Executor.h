#ifndef HOST_H
#define HOST_H

#include <memory>
#include <string>
#include <map>
#include <list>

class Executor {
    public:
        Executor() {}
        Executor(std::string host, std::string name) : 
            host(host), name(name), turn_off(false) {}
        
        const std::list<std::string> &get_running_tasks();
        
        bool get_turn_off();
        void set_turn_off(bool value);
        void place_task(std::string task);
        void conclude_task(std::string task);

    private:
        const std::string host;
        const std::string name;
        bool turn_off;
        std::list<std::string> tasks;
};

#endif
