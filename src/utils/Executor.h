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
            host(host), name(name) {}
        
        const std::list<std::string> &get_running_tasks();
        
        void place_task(std::string task);
        void conclude_task(std::string task);

    private:
        const std::string host;
        const std::string name;
        std::list<std::string> tasks;
};

#endif
