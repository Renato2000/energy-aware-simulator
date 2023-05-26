#ifndef ENERGY_METER_H
#define ENERGY_METER_H

#include <wrench-dev.h>

#include "utils/ClusterInfo.h"

class EnergyMeter : public wrench::Service {
public:
    EnergyMeter(wrench::WMS *wms,
               const std::vector<std::string> &hostnames,
               std::shared_ptr<ClusterInfo> cluster_info,
               double period);

    void stop() override;

private:
    int main() override;

    struct TaskStartTimeComparator {
        bool operator()(wrench::WorkflowTask *&lhs, wrench::WorkflowTask *&rhs);
    };

    void computePowerMeasurements(const std::string &hostname, std::set<wrench::WorkflowTask *> &tasks);

    bool processNextMessage(double timeout);

    wrench::WMS *wms;
    std::shared_ptr<ClusterInfo> cluster_info;
    double measurement_period;
    double time_to_next_measurement;
};

#endif //ENERGY_METER_H
