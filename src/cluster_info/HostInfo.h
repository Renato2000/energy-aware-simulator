#ifndef HOST_INFO_H
#define HOST_INFO_H

enum HostState {
    IN_USE,
    SUSPENDED,
    OFF
};

class HostInfo {
public:
    HostInfo(int num_cores);

    bool isAvailable();

    void useHost();

private:
    int num_cores;
    HostState state;
    std::string executor_name;  
};

#endif