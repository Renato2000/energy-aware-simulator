#ifndef HOSTS_H
#define HOSTS_H

#include <map>
#include <vector>

class Hosts {
    public:
        Hosts(std::string platform_file);

        const std::vector<std::string> get_hosts();
        int get_host_cores(std::string host);

    private:
        std::map<std::string, int> hosts;
};

#endif
