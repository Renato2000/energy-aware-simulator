#ifndef CLUSTER_INFO_H
#define CLUSTER_INFO_H

class ClusterInfo {
public:
    ClusterInfo(std::set<std::string> hosts);

    std::vector<std::string> getListHost();

    std::vector<std::string> getListAvailableHosts();

    std::string useHost(const std::string &host_name);

private:
    std::map<std::string *, const HostInfo *> hosts;    
};

#endif