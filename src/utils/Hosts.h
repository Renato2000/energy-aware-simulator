#ifndef HOSTS_H
#define HOSTS_H

#include <map>
#include <vector>

class Hosts {
    public:
        Hosts(std::string platform_file);

        const std::vector<std::string> get_hosts();
        int get_host_cores(std::string host);
        void use_core(std::string host);
        void free_core(std::string host);
        int get_available_cores(std::string host);

    private:
        std::map<std::string, int> hosts;
        std::map<std::string, int> available_cores;
};

#endif
