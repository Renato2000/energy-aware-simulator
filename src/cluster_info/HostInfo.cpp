#include "HostInfo.h"

HostInfo::HostInfo(std::shared_ptr<wrench::CloudComputeService> &cloud_service, int num_cores) {
    this->num_cores = num_cores;
    this->state = OFF;
    this->executor_name = nullptr;  
}

bool HostInfo::isAvailable() {
    return this->num_cores > 0; 
}

void HostInfo::useHost() {

}
